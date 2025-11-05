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

  // Build indices for O(1) lookups
  TMap<UConveyorBlockLogic *, int32> indexByPtr;
  indexByPtr.Reserve(conveyors.Num());
  for (int32 i = 0; i < conveyors.Num(); ++i) {
    if (conveyors[i]) indexByPtr.Add(conveyors[i], i);
  }

  struct FNodeData {
    UConveyorBlockLogic *BL = nullptr;
    UInventoryAccess *In = nullptr;
    UInventoryAccess *Out = nullptr;
    UBaseInventoryAccessor *ReceiverAcc = nullptr; // neighbor accessor to push into
    int32 NeighborConvIndex = -1;                  // -1 if neighbor is not a conveyor
    bool InputEmptyNow = true;
    bool OutputEmptyNow = true;
    bool InputReadyToMove = false;
    bool OutputReadyToPush = false;
  };

  const int32 N = conveyors.Num();
  TArray<FNodeData> nodes;
  nodes.SetNum(N);

  // Precompute per-node state and neighbor relations
  for (int32 i = 0; i < N; ++i) {
    UConveyorBlockLogic *bl = conveyors[i];
    if (!bl || !bl->GetSwitch_Implementation()) {
      nodes[i].BL = bl;
      continue;
    }

    nodes[i].BL = bl;
    nodes[i].In = bl->GetInputAccess_Implementation();
    nodes[i].Out = bl->GetOutputAccess_Implementation();
    nodes[i].InputEmptyNow = nodes[i].In ? nodes[i].In->IsEmpty() : true;
    nodes[i].OutputEmptyNow = nodes[i].Out ? nodes[i].Out->IsEmpty() : true;
    nodes[i].InputReadyToMove = (bl->input_path >= ConveyorConsts::SegmentLen) && !nodes[i].InputEmptyNow;
    nodes[i].OutputReadyToPush = (bl->output_path >= ConveyorConsts::SegmentLen) && !nodes[i].OutputEmptyNow;

    if (auto *senderOutAcc = bl->GetOutputAccessor()) {
      nodes[i].ReceiverAcc = senderOutAcc->GetOutsideNeighborSameTypeCached();
    }

    // Try to resolve neighbor conveyor via world position (robust and O(1) per node)
    UConveyorBlockLogic *neighborConv = nullptr;
    {
      const Vec3i neighborPos = bl->GetBlockPos() + RotateVector(bl->GetBlockQuat(), Side::Right);
      if (UBlockLogic *neighbor = bl->GetDim()->GetBlockLogic(neighborPos)) {
        if (UBlockLogic *root = neighbor->GetPartRootBlock()) {
          neighborConv = Cast<UConveyorBlockLogic>(root);
        }
      }
    }
    if (neighborConv) {
      if (int32 *idx = indexByPtr.Find(neighborConv)) {
        nodes[i].NeighborConvIndex = *idx;
      }
    }
  }

  // Build predecessor lists
  TArray<TArray<int32>> preds;
  preds.SetNum(N);
  for (int32 i = 0; i < N; ++i) {
    const int32 nb = nodes[i].NeighborConvIndex;
    if (nb >= 0 && nb < N) {
      preds[nb].Add(i);
    }
  }

  // Compute downstream acceptance via DFS with memoization (O(N))
  TArray<uint8> outState; // 0=unvisited,1=visiting,2=done
  outState.SetNumZeroed(N);
  TArray<bool> outClears;
  outClears.SetNumZeroed(N);
  TArray<bool> accept;
  accept.SetNumZeroed(N);

  TFunction<bool(int32)> ComputeOutClears = [&](int32 i) -> bool {
    if (i < 0 || i >= N) return false;
    if (!nodes[i].BL || !nodes[i].BL->GetSwitch_Implementation()) return nodes[i].OutputEmptyNow;

    if (outState[i] == 2) return outClears[i];
    if (outState[i] == 1) {
      // Cycle: conservatively assume no clearing besides already empty output
      accept[i] = false;
      outClears[i] = nodes[i].OutputEmptyNow;
      outState[i] = 2;
      return outClears[i];
    }

    outState[i] = 1;

    bool acc = false;
    if (!nodes[i].ReceiverAcc) {
      acc = false;
    } else if (nodes[i].NeighborConvIndex < 0) {
      if (UInventoryAccess *recvIn = nodes[i].ReceiverAcc->GetInput()) {
        acc = recvIn->IsEmpty();
      } else {
        acc = false;
      }
    } else {
      const int32 j = nodes[i].NeighborConvIndex;
      const bool recvAccepts = nodes[j].InputEmptyNow || (nodes[j].InputReadyToMove && ComputeOutClears(j));
      acc = recvAccepts;
    }

    accept[i] = acc;
    outClears[i] = nodes[i].OutputEmptyNow || (nodes[i].OutputReadyToPush && acc);
    outState[i] = 2;
    return outClears[i];
  };

  for (int32 i = 0; i < N; ++i) {
    if (outState[i] == 0) {
      ComputeOutClears(i);
    }
  }

  // Choose one sender per receiver deterministically
  TMap<UBaseInventoryAccessor *, UConveyorBlockLogic *> chosenSenderByReceiver;
  chosenSenderByReceiver.Reserve(N);
  for (int32 i = 0; i < N; ++i) {
    auto *bl = nodes[i].BL;
    if (!bl || !bl->GetSwitch_Implementation()) continue;
    if (!nodes[i].ReceiverAcc) continue;
    if (!(nodes[i].OutputReadyToPush && accept[i])) continue;

    UConveyorBlockLogic *&current = chosenSenderByReceiver.FindOrAdd(nodes[i].ReceiverAcc);
    if (!current || bl < current) {
      current = bl;
    }
  }

  // Build downstream-to-upstream processing order (post-order DFS)
  TArray<uint8> vis; // 0=unvisited,1=visiting,2=done
  vis.SetNumZeroed(N);
  TArray<int32> order;
  order.Reserve(N);
  TFunction<void(int32)> DfsOrder = [&](int32 i) {
    if (i < 0 || i >= N) return;
    if (!nodes[i].BL || !nodes[i].BL->GetSwitch_Implementation()) return;
    if (vis[i]) return;
    vis[i] = 1;
    const int32 j = nodes[i].NeighborConvIndex;
    if (j >= 0 && j < N) {
      DfsOrder(j);
    }
    vis[i] = 2;
    order.Add(i);
  };
  for (int32 i = 0; i < N; ++i) {
    DfsOrder(i);
  }

  // Single downstream-to-upstream sweep: push then internal move for each node
  TSet<UConveyorBlockLogic *> pushedSenders;
  pushedSenders.Reserve(chosenSenderByReceiver.Num());
  for (int32 k = 0; k < order.Num(); ++k) {
    const int32 i = order[k];
    UConveyorBlockLogic *bl = nodes[i].BL;
    if (!bl || !bl->GetSwitch_Implementation()) continue;

    // Attempt push if selected for its receiver and logically accepted
    bool didPush = false;
    if (nodes[i].ReceiverAcc && nodes[i].OutputReadyToPush && accept[i]) {
      UConveyorBlockLogic *chosen = chosenSenderByReceiver.FindRef(nodes[i].ReceiverAcc);
      if (chosen == bl) {
        auto *senderOut = nodes[i].Out;
        if (senderOut && !senderOut->IsEmpty()) {
#ifdef EVOSPACE_ITEMS_RENDERING
          bool pushed = false;
          if (bl->GetActor()) {
            pushed = nodes[i].ReceiverAcc->PushWithData(senderOut, 1, MoveTemp(bl->mItemInstancing2));
            if (!pushed) {
              bl->mItemInstancing2.UpdateData(Vec3i::Zero(), 0);
            }
          } else {
            pushed = nodes[i].ReceiverAcc->Push(senderOut, false);
          }
#else
          const bool pushed = nodes[i].ReceiverAcc->Push(senderOut, false);
#endif
          if (pushed) {
            bl->output_path = 0;
            pushedSenders.Add(bl);
            didPush = true;
          }
        }
      }
    }

    // Internal move after push so upstream can push into us later in this sweep
    auto *inAcc = nodes[i].In;
    auto *outAcc = nodes[i].Out;
    if (inAcc && outAcc) {
      const bool outputCleared = outAcc->IsEmpty() || didPush;
      if (nodes[i].InputReadyToMove && outputCleared) {
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


