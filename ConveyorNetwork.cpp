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
#include "Evospace/PerformanceStat.h"

DECLARE_CYCLE_STAT(TEXT("Tick ConveyorNetwork"), STAT_TickConveyorNetwork, STATGROUP_BLOCKLOGIC);

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
  ReceiverExternalInput.SetNum(n);
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
    ReceiverExternalInput[i] = nullptr;
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

    if (ReceiverAccessors[i] && NeighborIndex[i] < 0) {
      ReceiverExternalInput[i] = ReceiverAccessors[i]->GetInput();
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
  TickStates.SetNum(n);
  Accept.SetNum(n);
  OutClears.SetNum(n);
  ActivePostOrder.Reset();
  ActivePostOrder.Reserve(n);
}

void UConveyorNetwork::Tick() {
  SCOPE_CYCLE_COUNTER(STAT_TickConveyorNetwork);

  const TArray<UConveyorBlockLogic *> &conveyors = Conveyors;
  const int32 conveyorCount = conveyors.Num();

  for (UConveyorBlockLogic *bl : conveyors) {
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

    // Visualization: ensure instances reflect current inventories (auto-spawn)
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

  const int32 orderCount = PostOrder.Num();
  TickStates.SetNum(conveyorCount);
  Accept.SetNum(conveyorCount);
  OutClears.SetNum(conveyorCount);
  ActivePostOrder.Reset();
  ActivePostOrder.Reserve(orderCount);

  for (int32 orderIdx = 0; orderIdx < orderCount; ++orderIdx) {
    const int32 index = PostOrder[orderIdx];
    FConveyorTickState &state = TickStates[index];
    state.InputEmpty = true;
    state.OutputEmpty = true;
    state.InputReady = false;
    state.OutputReady = false;

    UConveyorBlockLogic *bl = conveyors[index];
    UInventoryAccess *inputAccess = CachedInput[index];
    UInventoryAccess *outputAccess = CachedOutput[index];

    const bool inputEmpty = !inputAccess || inputAccess->IsEmpty();
    const bool outputEmpty = !outputAccess || outputAccess->IsEmpty();

    state.InputEmpty = inputEmpty;
    state.OutputEmpty = outputEmpty;

    if (!bl || !bl->GetSwitch_Implementation()) {
      Accept[index] = false;
      OutClears[index] = outputEmpty;
      continue;
    }

    const bool inputReady = (bl->input_path >= ConveyorConsts::SegmentLen) && !inputEmpty;
    const bool outputReady = (bl->output_path >= ConveyorConsts::SegmentLen) && !outputEmpty;
    state.InputReady = inputReady;
    state.OutputReady = outputReady;

    bool accept = false;
    UBaseInventoryAccessor *receiverAccessor = ReceiverAccessors[index];
    const int32 neighborIndex = NeighborIndex[index];
    if (receiverAccessor) {
      if (neighborIndex < 0) {
        UInventoryAccess *receiverInput = ReceiverExternalInput[index];
        accept = receiverInput ? receiverInput->IsEmpty() : false;
      } else {
        const FConveyorTickState &downstreamState = TickStates[neighborIndex];
        const bool downstreamAccepts = downstreamState.InputEmpty || (downstreamState.InputReady && OutClears[neighborIndex]);
        accept = downstreamAccepts;
      }
    }

    Accept[index] = accept;
    OutClears[index] = outputEmpty || (outputReady && accept);

    if (!inputEmpty || !outputEmpty || inputReady || outputReady) {
      ActivePostOrder.Add(index);
    }
  }

  if (ChosenSenderByReceiver.Num() != UniqueReceiverCount) {
    ChosenSenderByReceiver.SetNum(UniqueReceiverCount);
  }
  for (int32 receiverIdx = 0; receiverIdx < ChosenSenderByReceiver.Num(); ++receiverIdx) {
    ChosenSenderByReceiver[receiverIdx] = nullptr;
  }

  for (int32 index : ActivePostOrder) {
    UConveyorBlockLogic *bl = conveyors[index];
    if (!bl || !bl->GetSwitch_Implementation()) continue;

    const int32 receiverId = ReceiverId[index];
    if (receiverId < 0) continue;
    if (!(TickStates[index].OutputReady && Accept[index])) continue;

    UConveyorBlockLogic *&chosen = ChosenSenderByReceiver[receiverId];
    if (!chosen || bl < chosen) {
      chosen = bl;
    }
  }

  for (int32 index : ActivePostOrder) {
    UConveyorBlockLogic *bl = conveyors[index];
    if (!bl || !bl->GetSwitch_Implementation()) continue;

    UInventoryAccess *inputAccess = CachedInput[index];
    UInventoryAccess *outputAccess = CachedOutput[index];

    bool didPush = false;
    const int32 receiverId = ReceiverId[index];
    if (receiverId >= 0 && ReceiverAccessors[index] && TickStates[index].OutputReady && Accept[index]) {
      if (ChosenSenderByReceiver[receiverId] == bl) {
        if (outputAccess && !TickStates[index].OutputEmpty) {
#ifdef EVOSPACE_ITEMS_RENDERING
          bool pushed = false;
          if (bl->GetActor()) {
            auto ret = ReceiverAccessors[index]->PushWithData(outputAccess, 1, MoveTemp(bl->mItemInstancing2));
            if (ret.has_value()) {
              // push failed: keep and stop movement
              bl->mItemInstancing2 = MoveTemp(*ret);
              bl->mItemInstancing2.UpdateData(Vec3i::Zero(), 0);
              pushed = false;
            } else {
              // push succeeded: we no longer own handle
              bl->mItemInstancing2.Reset();
              pushed = true;
            }
          } else {
            pushed = ReceiverAccessors[index]->Push(outputAccess, false);
          }
#else
          const bool pushed = ReceiverAccessors[index]->Push(outputAccess, false);
#endif
          if (pushed) {
            bl->output_path = 0;
            didPush = true;
          }
        }
      }
    }

    if (inputAccess && outputAccess) {
      bool outputCleared = TickStates[index].OutputEmpty || didPush;
      if (!outputCleared) {
        outputCleared = outputAccess ? outputAccess->IsEmpty() : true;
      }

      if (TickStates[index].InputReady && outputCleared) {
        if (UInventoryLibrary::TryMoveFromAny(outputAccess, inputAccess)) {
          bl->input_path -= ConveyorConsts::SegmentLen;
          if (bl->input_path < 0) bl->input_path = 0;
          bl->output_path = 0;
#ifdef EVOSPACE_ITEMS_RENDERING
          if (bl->GetActor()) {
            bl->mItemInstancing2 = MoveTemp(bl->mItemInstancing);
            bl->mItemInstancing2.UpdateTransform(bl->GetTransformLocation());
            bl->mItemInstancing2.UpdateData(RotateVector(bl->GetBlockQuat(), Side::Right), ConveyorConsts::StepForLevel(bl->GetStaticBlock()->Level));
          }
#endif
        }
      }
    }
  }
}


