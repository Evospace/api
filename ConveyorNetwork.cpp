// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#include "Public/ConveyorNetwork.h"

#include "Evospace/Blocks/Vanilla/ConveyorBlockLogic.h"
#include "Evospace/CoordinateSystem.h"
#include "Evospace/Item/InventoryLibrary.h"
#include "Public/BaseInventoryAccessor.h"
#include "Public/MainGameInstance.h"
#include "DrawDebugHelpers.h"
#include "Public/ConveyorConsts.h"
#include "Public/Dimension.h"
#include "Public/StaticBlock.h"

namespace {
struct FSenderReceiverPair {
  UConveyorBlockLogic *Sender;
  UBaseInventoryAccessor *ReceiverAcc; // any item-accessor in front, not only conveyors
};
}

void UConveyorNetwork::RebuildCache() {

  const int32 n = Conveyors.Num();

  NeighborIndex.SetNum(n);
  ReceiverAccessors.SetNum(n);
  ReceiverId.SetNum(n);
  CachedInput.SetNum(n);
  CachedOutput.SetNum(n);
  PostOrder.Reset();
  PostOrder.Reserve(n);

  // Build pointer->index map for conveyors
  TMap<UConveyorBlockLogic *, int32> indexByPtr;
  indexByPtr.Reserve(n);
  for (int32 i = 0; i < n; ++i) {
    if (Conveyors[i]) indexByPtr.Add(Conveyors[i], i);
  }

  // Assign static neighbor indices and receiver accessors; cache IO pointers
  for (int32 i = 0; i < n; ++i) {
    UConveyorBlockLogic *bl = Conveyors[i];
    NeighborIndex[i] = -1;
    ReceiverAccessors[i] = nullptr;
    ReceiverId[i] = -1;
    CachedInput[i] = nullptr;
    CachedOutput[i] = nullptr;
    if (!bl) continue;

    CachedInput[i] = bl->GetInputAccess_Implementation();
    CachedOutput[i] = bl->GetOutputAccess_Implementation();

    if (auto *outAcc = bl->GetOutputAccessor()) {
      ReceiverAccessors[i] = outAcc->GetOutsideNeighborSameTypeCached();
    }

    // Downstream neighbor conveyor index (by world position)
    const Vec3i neighborPos = bl->GetBlockPos() + RotateVector(bl->GetBlockQuat(), Side::Right);
    if (UBlockLogic *neighbor = bl->GetDim()->GetBlockLogic(neighborPos)) {
      if (UBlockLogic *root = neighbor->GetPartRootBlock()) {
        if (UConveyorBlockLogic *neighborConv = Cast<UConveyorBlockLogic>(root)) {
          if (int32 *idx = indexByPtr.Find(neighborConv)) {
            NeighborIndex[i] = *idx;
          }
        }
      }
    }
  }

  // Compact receiver accessors into IDs to avoid hash maps in Tick
  TMap<UBaseInventoryAccessor *, int32> receiverToId;
  receiverToId.Reserve(n);
  int32 nextId = 0;
  for (int32 i = 0; i < n; ++i) {
    UBaseInventoryAccessor *acc = ReceiverAccessors[i];
    if (!acc) { ReceiverId[i] = -1; continue; }
    int32 *found = receiverToId.Find(acc);
    if (found) {
      ReceiverId[i] = *found;
    } else {
      ReceiverId[i] = nextId;
      receiverToId.Add(acc, nextId);
      ++nextId;
    }
  }
  UniqueReceiverCount = nextId;
  ChosenSenderByReceiver.SetNum(UniqueReceiverCount);
  for (int32 i = 0; i < UniqueReceiverCount; ++i) ChosenSenderByReceiver[i] = nullptr;

  // Build downstream-to-upstream post-order once
  TmpVisited.SetNumZeroed(n);
  TFunction<void(int32)> dfs = [&](int32 i) {
    if (i < 0 || i >= n) return;
    if (TmpVisited[i]) return;
    TmpVisited[i] = 1;
    const int32 j = NeighborIndex[i];
    if (j >= 0) dfs(j);
    TmpVisited[i] = 2;
    PostOrder.Add(i);
  };
  for (int32 i = 0; i < n; ++i) dfs(i);

  // Prepare working buffers
  InputEmpty.SetNum(n);
  OutputEmpty.SetNum(n);
  InputReady.SetNum(n);
  OutputReady.SetNum(n);
  Accept.SetNum(n);
  OutClears.SetNum(n);
}

void UConveyorNetwork::Tick() {


  // Collect snapshot to avoid mutation order dependence
  TArray<UConveyorBlockLogic *> conveyors = Conveyors;

  for (auto *bl : conveyors) {
    if (!bl) continue;

    // Advance paths deterministically here (independent of block tick order)
    if (bl->GetSwitch_Implementation()) {
      const int32 step = ConveyorConsts::StepForLevel(bl->GetStaticBlock()->Level);
      if (!bl->GetInputAccess_Implementation()->IsEmpty()) {
        bl->input_path += step;
      }
      if (!bl->GetOutputAccess_Implementation()->IsEmpty()) {
        bl->output_path += step;
      }
    }

    // Debug draw connections between conveyors: output direction to next conveyor
    // const FColor color = UMainGameInstance::VoidToColor(this).ToFColor(true);
    // Debug line
    // const Vec3i pos = bl->GetBlockPos();
    // const Vec3i outPos = pos + RotateVector(bl->GetBlockQuat(), Side::Right);
    // const FVector start = cs::WBtoWd(pos) + FVector(gCubeSize / 2.0);
    // const FVector end = cs::WBtoWd(outPos) + FVector(gCubeSize / 2.0);
    // DrawDebugLine(bl->GetWorld(), start, end, color, false, 0.0f, 0, 4.0f);

    // Visualization: ensure instances reflect current inventories
    {
      bool is_valid_actor = bl->GetActor() != nullptr;
#ifndef EVOSPACE_ITEMS_RENDERING
      is_valid_actor = false;
#endif
      auto *inAcc = bl->GetInputAccess_Implementation();
      auto *outAcc = bl->GetOutputAccess_Implementation();
      if (is_valid_actor && inAcc && outAcc) {
        // Output visual instance lifecycle (at right side)
        if (bl->mItemInstancing2.IsEmpty() && !outAcc->IsEmpty()) {
          auto rt = bl->GetTransformLocation();
          rt.AddToTranslation(RotateVector(bl->GetBlockQuat(), Side::Right).vec() * gCubeSize / 2.f);
          bl->mItemInstancing2 = bl->GetDim()->AddItemInstance(outAcc->_Get(0).mItem, rt);
        } else if (outAcc->IsEmpty()) {
          bl->mItemInstancing2.Reset();
          bl->output_path = 0;
        }

        // Input visual instance lifecycle (center)
        if (bl->mItemInstancing.IsEmpty() && !inAcc->IsEmpty()) {
          auto rt = bl->GetTransformLocation();
          bl->mItemInstancing = bl->GetDim()->AddItemInstance(inAcc->_Get(0).mItem, rt);
        } else if (inAcc->IsEmpty()) {
          bl->mItemInstancing.Reset();
          bl->input_path = 0;
        }

        // If switched off, stop movement
        if (!bl->GetSwitch_Implementation()) {
          bl->mItemInstancing2.UpdateData(Vec3i::Zero(), 0);
          bl->mItemInstancing.UpdateData(Vec3i::Zero(), 0);
        }
      }
    }

    // Skip switched off for logic
    if (!bl->GetSwitch_Implementation()) continue;

    auto *inAcc = bl->GetInputAccess_Implementation();
    auto *outAcc = bl->GetOutputAccess_Implementation();
    if (!inAcc || !outAcc) continue;

    // No collection here; we will resolve moves in a single dependency-aware pass below
  }

  const int32 N = conveyors.Num();

  // Resize working buffers if topology changed without rebuild (safety)
  if (InputEmpty.Num() != N) {
    InputEmpty.SetNum(N);
    OutputEmpty.SetNum(N);
    InputReady.SetNum(N);
    OutputReady.SetNum(N);
    Accept.SetNum(N);
    OutClears.SetNum(N);
  }
  if (PostOrder.Num() != N || NeighborIndex.Num() != N) {
    // Topology desync: rebuild
    RebuildCache();
  }

  // Gather dynamic state per node
  for (int32 i = 0; i < N; ++i) {
    UConveyorBlockLogic *bl = conveyors[i];
    if (!bl || !bl->GetSwitch_Implementation()) {
      InputEmpty[i] = true;
      OutputEmpty[i] = true;
      InputReady[i] = false;
      OutputReady[i] = false;
      continue;
    }
    UInventoryAccess *inAcc = CachedInput.IsValidIndex(i) ? CachedInput[i] : bl->GetInputAccess_Implementation();
    UInventoryAccess *outAcc = CachedOutput.IsValidIndex(i) ? CachedOutput[i] : bl->GetOutputAccess_Implementation();
    const bool inEmpty = inAcc ? inAcc->IsEmpty() : true;
    const bool outEmpty = outAcc ? outAcc->IsEmpty() : true;
    InputEmpty[i] = inEmpty;
    OutputEmpty[i] = outEmpty;
    InputReady[i] = (bl->input_path >= ConveyorConsts::SegmentLen) && !inEmpty;
    OutputReady[i] = (bl->output_path >= ConveyorConsts::SegmentLen) && !outEmpty;
  }

  // Downstream-to-upstream DP for accept/outClears
  for (int32 k = 0; k < PostOrder.Num(); ++k) {
    const int32 i = PostOrder[k];
    UConveyorBlockLogic *bl = conveyors[i];
    if (!bl || !bl->GetSwitch_Implementation()) {
      Accept[i] = false;
      OutClears[i] = OutputEmpty.IsValidIndex(i) ? OutputEmpty[i] : true;
      continue;
    }
    bool acc = false;
    UBaseInventoryAccessor *recvAcc = ReceiverAccessors.IsValidIndex(i) ? ReceiverAccessors[i] : nullptr;
    const int32 j = NeighborIndex.IsValidIndex(i) ? NeighborIndex[i] : -1;
    if (!recvAcc) {
      acc = false;
    } else if (j < 0) {
      if (UInventoryAccess *recvIn = recvAcc->GetInput()) {
        acc = recvIn->IsEmpty();
      } else {
        acc = false;
      }
    } else {
      const bool recvAccepts = InputEmpty[j] || (InputReady[j] && OutClears[j]);
      acc = recvAccepts;
    }
    Accept[i] = acc;
    OutClears[i] = OutputEmpty[i] || (OutputReady[i] && acc);
  }

  // Choose one sender per receiver deterministically (by pointer address)
  if (ChosenSenderByReceiver.Num() != UniqueReceiverCount) {
    ChosenSenderByReceiver.SetNum(UniqueReceiverCount);
  }
  for (int32 r = 0; r < ChosenSenderByReceiver.Num(); ++r) ChosenSenderByReceiver[r] = nullptr;
  for (int32 i = 0; i < N; ++i) {
    UConveyorBlockLogic *bl = conveyors[i];
    if (!bl || !bl->GetSwitch_Implementation()) continue;
    const int32 rid = ReceiverId.IsValidIndex(i) ? ReceiverId[i] : -1;
    if (rid < 0) continue;
    if (!(OutputReady[i] && Accept[i])) continue;
    UConveyorBlockLogic *&slot = ChosenSenderByReceiver[rid];
    if (!slot || bl < slot) slot = bl;
  }

  // Process in cached downstream-to-upstream order: push then internal move
  for (int32 k = 0; k < PostOrder.Num(); ++k) {
    const int32 i = PostOrder[k];
    UConveyorBlockLogic *bl = conveyors[i];
    if (!bl || !bl->GetSwitch_Implementation()) continue;
    UInventoryAccess *inAcc = CachedInput[i];
    UInventoryAccess *outAcc = CachedOutput[i];

    bool didPush = false;
    const int32 rid = ReceiverId[i];
    if (rid >= 0 && ReceiverAccessors[i] && OutputReady[i] && Accept[i]) {
      if (ChosenSenderByReceiver[rid] == bl) {
        if (outAcc && !outAcc->IsEmpty()) {
#ifdef EVOSPACE_ITEMS_RENDERING
          bool pushed = false;
          if (bl->GetActor()) {
            pushed = ReceiverAccessors[i]->PushWithData(outAcc, 1, MoveTemp(bl->mItemInstancing2));
            if (!pushed) {
              bl->mItemInstancing2.UpdateData(Vec3i::Zero(), 0);
            }
          } else {
            pushed = ReceiverAccessors[i]->Push(outAcc, false);
          }
#else
          const bool pushed = ReceiverAccessors[i]->Push(outAcc, false);
#endif
          if (pushed) {
            bl->output_path = 0;
            didPush = true;
          }
        }
      }
    }

    if (inAcc && outAcc) {
      const bool outputCleared = outAcc->IsEmpty() || didPush;
      if (InputReady[i] && outputCleared) {
        if (UInventoryLibrary::TryMoveFromAny(outAcc, inAcc)) {
          bl->input_path -= ConveyorConsts::SegmentLen;
          if (bl->input_path < 0) bl->input_path = 0;
          bl->output_path = 0;
#ifdef EVOSPACE_ITEMS_RENDERING
          if (bl->GetActor()) {
            bl->mItemInstancing2 = MoveTemp(bl->mItemInstancing);
            bl->mItemInstancing2.UpdateData(RotateVector(bl->GetBlockQuat(), Side::Right), ConveyorConsts::StepForLevel(bl->GetStaticBlock()->Level));
            bl->mItemInstancing2.UpdateTransform(bl->GetTransformLocation());
          }
#endif
        }
      }
    }
  }
}


