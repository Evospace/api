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
  // Debug draw connections between conveyors: output direction to next conveyor
  const FColor color = UMainGameInstance::VoidToColor(this).ToFColor(true);

  // Collect snapshot to avoid mutation order dependence
  TArray<UConveyorBlockLogic *> conveyors = Conveyors;

  // Candidate internal moves and neighbor pushes
  TArray<UConveyorBlockLogic *> InternalMoves;
  TArray<FSenderReceiverPair> NeighborMoves;

  for (auto *bl : conveyors) {
    if (!bl) continue;

    // Advance paths deterministically here (independent of block tick order)
    if (bl->GetSwitch_Implementation()) {
      const int32 step = ConveyorConsts::StepPerTick;
      if (!bl->GetInputAccess_Implementation()->IsEmpty()) {
        bl->input_path += step;
      }
      if (!bl->GetOutputAccess_Implementation()->IsEmpty()) {
        bl->output_path += step;
      }
    }

    // Debug line
    const Vec3i pos = bl->GetBlockPos();
    const Vec3i outPos = pos + RotateVector(bl->GetBlockQuat(), Side::Right);
    const FVector start = cs::WBtoWd(pos) + FVector(gCubeSize / 2.0);
    const FVector end = cs::WBtoWd(outPos) + FVector(gCubeSize / 2.0);
    DrawDebugLine(bl->GetWorld(), start, end, color, false, 0.0f, 0, 4.0f);

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

    // Internal move candidate: input ready to move to output, output empty
    if (bl->input_path >= ConveyorConsts::SegmentLen && !inAcc->IsEmpty() && outAcc->IsEmpty()) {
      InternalMoves.Add(bl);
    }

    // Neighbor move candidate: output ready and has an item
    if (bl->output_path >= ConveyorConsts::SegmentLen && !outAcc->IsEmpty()) {
      if (auto *senderOutAcc = bl->GetOutputAccessor()) {
        if (auto *receiverAcc = senderOutAcc->GetOutsideNeighborSameTypeCached()) {
          NeighborMoves.Add({ bl, receiverAcc });
        }
      }
    }
  }

  // Resolve conflicts deterministically for neighbor moves (by sender/receiver pointer)
  NeighborMoves.Sort([](const FSenderReceiverPair &A, const FSenderReceiverPair &B) {
    if (A.ReceiverAcc == B.ReceiverAcc) return A.Sender < B.Sender;
    return A.ReceiverAcc < B.ReceiverAcc;
  });
  TSet<UBaseInventoryAccessor *> takenReceivers;
  TArray<FSenderReceiverPair> MovesToApply;
  for (const auto &p : NeighborMoves) {
    bool existed = false;
    takenReceivers.Add(p.ReceiverAcc, &existed);
    if (!existed) {
      MovesToApply.Add(p);
    }
  }

  // Apply internal moves (and visuals)
  for (auto *bl : InternalMoves) {
    auto *inAcc = bl->GetInputAccess_Implementation();
    auto *outAcc = bl->GetOutputAccess_Implementation();
    if (!inAcc || !outAcc) continue;
    if (UInventoryLibrary::TryMoveFromAny(outAcc, inAcc)) {
      bl->input_path -= ConveyorConsts::SegmentLen;
      if (bl->input_path < 0) bl->input_path = 0;
      bl->output_path = 0;

#ifdef EVOSPACE_ITEMS_RENDERING
      if (bl->GetActor()) {
        // Move visual from center to right side within the block
        bl->mItemInstancing2 = MoveTemp(bl->mItemInstancing);
        bl->mItemInstancing2.UpdateData(RotateVector(bl->GetBlockQuat(), Side::Right), ConveyorConsts::StepPerTick);
        bl->mItemInstancing2.UpdateTransform(bl->GetTransformLocation());
      }
#endif
    }
  }

  // Apply neighbor moves (and visuals)
  for (const auto &mv : MovesToApply) {
    auto *senderOut = mv.Sender->GetOutputAccess_Implementation();
    if (!senderOut || !mv.ReceiverAcc) continue;

#ifdef EVOSPACE_ITEMS_RENDERING
    bool pushed = false;
    if (mv.Sender->GetActor()) {
      // Transfer with visual handle if available
      pushed = mv.ReceiverAcc->PushWithData(senderOut, 1, MoveTemp(mv.Sender->mItemInstancing2));
      if (!pushed) {
        // Stop animation if transfer failed this tick
        mv.Sender->mItemInstancing2.UpdateData(Vec3i::Zero(), 0);
      }
    } else {
      pushed = mv.ReceiverAcc->Push(senderOut, false);
    }
#else
    const bool pushed = mv.ReceiverAcc->Push(senderOut, false);
#endif

    if (pushed) {
      const int32 leftover = mv.Sender->output_path - ConveyorConsts::SegmentLen;
      mv.Sender->output_path = FMath::Max(0, leftover);
      // If receiver is a conveyor, seed its input progress
      if (auto *recvBl = Cast<UConveyorBlockLogic>(mv.ReceiverAcc->GetOuter())) {
        recvBl->input_path = FMath::Min(recvBl->input_path + FMath::Max(0, leftover), ConveyorConsts::SegmentLen - 1);
      }
    }
  }
}


