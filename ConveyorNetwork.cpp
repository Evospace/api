// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#include "Public/ConveyorNetwork.h"

#include "Evospace/Blocks/Vanilla/ConveyorBlockLogic.h"
#include "Evospace/CoordinateSystem.h"
#include "Evospace/Item/InventoryLibrary.h"
#include "Public/BaseInventoryAccessor.h"
#include "Public/MainGameInstance.h"
#include "DrawDebugHelpers.h"
#include "Public/ConveyorConsts.h"

namespace {
struct FSenderReceiverPair {
  UConveyorBlockLogic *Sender;
  UConveyorBlockLogic *Receiver;
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

    // Skip switched off
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
      const Vec3i neighborPos = bl->GetBlockPos() + RotateVector(bl->GetBlockQuat(), Side::Right);
      if (UBlockLogic *neighbor = bl->GetDim()->GetBlockLogic(neighborPos)) {
        if (UBlockLogic *root = neighbor->GetPartRootBlock()) {
          if (UConveyorBlockLogic *receiver = Cast<UConveyorBlockLogic>(root)) {
            NeighborMoves.Add({ bl, receiver });
          }
        }
      }
    }
  }

  // Resolve conflicts deterministically for neighbor moves (by sender/receiver pointer)
  NeighborMoves.Sort([](const FSenderReceiverPair &A, const FSenderReceiverPair &B) {
    if (A.Receiver == B.Receiver) return A.Sender < B.Sender;
    return A.Receiver < B.Receiver;
  });
  TSet<UConveyorBlockLogic *> takenReceivers;
  TArray<FSenderReceiverPair> MovesToApply;
  for (const auto &p : NeighborMoves) {
    bool existed = false;
    takenReceivers.Add(p.Receiver, &existed);
    if (!existed) {
      MovesToApply.Add(p);
    }
  }

  // Apply internal moves
  for (auto *bl : InternalMoves) {
    auto *inAcc = bl->GetInputAccess_Implementation();
    auto *outAcc = bl->GetOutputAccess_Implementation();
    if (!inAcc || !outAcc) continue;
    if (UInventoryLibrary::TryMoveFromAny(outAcc, inAcc)) {
      bl->input_path -= ConveyorConsts::SegmentLen;
      if (bl->input_path < 0) bl->input_path = 0;
      bl->output_path = 0;
      bl->NotifyInternalMoveVisual();
    }
  }

  // Apply neighbor moves
  for (const auto &mv : MovesToApply) {
    auto *senderOut = mv.Sender->GetOutputAccess_Implementation();
    if (!senderOut) continue;
    if (auto *senderOutAcc = mv.Sender->GetOutputAccessor()) {
      if (auto *receiverAcc = senderOutAcc->GetOutsideNeighborSameTypeCached()) {
        if (receiverAcc->Push(senderOut, false)) {
        const int32 leftover = mv.Sender->output_path - ConveyorConsts::SegmentLen;
        mv.Sender->output_path = FMath::Max(0, leftover);
        // Optionally seed receiver progress with leftover
        mv.Receiver->input_path = FMath::Min(mv.Receiver->input_path + FMath::Max(0, leftover), ConveyorConsts::SegmentLen - 1);
        mv.Sender->NotifyOutputPushedVisual();
        }
      }
    }
  }
}


