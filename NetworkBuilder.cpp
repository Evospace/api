#include "Public/NetworkBuilder.h"

#include "Evospace/Blocks/Vanilla/ConveyorBlockLogic.h"
#include "Public/ConductorBlockLogic.h"
#include "Public/Dimension.h"
#include "Qr/Vector.h"

void UNetworkBuilder::Initialize(ADimension *inOwner) {
  ownerDimension = inOwner;
}

void UNetworkBuilder::OnBlockAdded(UBlockLogic *block) {
  MarkBlockDirty(block);
}

void UNetworkBuilder::OnBlockRemoved(const FVector3i &pos) {
  MarkPositionDirty(pos);
}

void UNetworkBuilder::OnBlockChanged(UBlockLogic *block) {
  MarkBlockDirty(block);
}

void UNetworkBuilder::Flush() {
  if (bFlushing) {
    return;
  }
  if (!ownerDimension.IsValid()) {
    pendingConductors.Empty();
    pendingConveyors.Empty();
    return;
  }

  bFlushing = true;
  RebuildConductors();
  RebuildConveyors();
  bFlushing = false;
}

void UNetworkBuilder::MarkBlockDirty(UBlockLogic *block) {
  if (!block) {
    return;
  }
  if (!ownerDimension.IsValid()) {
    return;
  }

  UBlockLogic *rootBlock = block->GetPartRootBlock();
  if (!rootBlock) {
    rootBlock = block;
  }

  if (UConductorBlockLogic *conductor = Cast<UConductorBlockLogic>(rootBlock)) {
    pendingConductors.Add(conductor);
  }
  if (UConveyorBlockLogic *conveyor = Cast<UConveyorBlockLogic>(rootBlock)) {
    pendingConveyors.Add(conveyor);
  }
}

void UNetworkBuilder::MarkPositionDirty(const FVector3i &pos) {
  if (!ownerDimension.IsValid()) {
    return;
  }

  if (UBlockLogic *block = ownerDimension->GetBlockLogic(pos)) {
    MarkBlockDirty(block);
  }
}

void UNetworkBuilder::RebuildConductors() {
  if (pendingConductors.Num() == 0) {
    return;
  }
  if (!ownerDimension.IsValid()) {
    pendingConductors.Empty();
    return;
  }

  TSet<TObjectPtr<UConductorBlockLogic>> snapshot = MoveTemp(pendingConductors);
  pendingConductors.Empty();

  TSet<TObjectPtr<UConductorBlockLogic>> rebuiltBlocks;

  for (UConductorBlockLogic *conductor : snapshot) {
    if (!conductor) {
      continue;
    }
    UBlockLogic *liveBlock = ownerDimension->GetBlockLogic(conductor->GetBlockPos());
    if (!liveBlock) {
      continue;
    }
    UBlockLogic *liveRoot = liveBlock->GetPartRootBlock();
    if (!liveRoot) {
      liveRoot = liveBlock;
    }
    if (liveRoot != conductor) {
      continue;
    }

    // Safe dedup: skip only blocks already confirmed rebuilt in this flush.
    // Do not infer via current network membership because it may still point to
    // stale pre-split topology and can hide mandatory rebuilds.
    if (rebuiltBlocks.Contains(conductor)) {
      continue;
    }

    conductor->RebuildNetworkImmediate();
    if (UBlockNetwork *rebuiltNetwork = conductor->GetNetwork()) {
      for (UConductorBlockLogic *wire : rebuiltNetwork->mWires) {
        rebuiltBlocks.Add(wire);
      }
    } else {
      rebuiltBlocks.Add(conductor);
    }
  }
}

void UNetworkBuilder::RebuildConveyors() {
  if (pendingConveyors.Num() == 0) {
    return;
  }
  if (!ownerDimension.IsValid()) {
    pendingConveyors.Empty();
    return;
  }

  TSet<TObjectPtr<UConveyorBlockLogic>> snapshot = MoveTemp(pendingConveyors);
  pendingConveyors.Empty();

  TSet<TObjectPtr<UConveyorBlockLogic>> rebuiltBlocks;

  for (UConveyorBlockLogic *conveyor : snapshot) {
    if (!conveyor) {
      continue;
    }
    UBlockLogic *liveBlock = ownerDimension->GetBlockLogic(conveyor->GetBlockPos());
    if (!liveBlock) {
      continue;
    }
    UBlockLogic *liveRoot = liveBlock->GetPartRootBlock();
    if (!liveRoot) {
      liveRoot = liveBlock;
    }
    if (liveRoot != conveyor) {
      continue;
    }

    // Safe dedup for split scenarios, same rationale as conductors above.
    if (rebuiltBlocks.Contains(conveyor)) {
      continue;
    }

    conveyor->RebuildNetworkImmediate();
    if (conveyor->mNetwork) {
      for (UConveyorBlockLogic *member : conveyor->mNetwork->Conveyors) {
        rebuiltBlocks.Add(member);
      }
    } else {
      rebuiltBlocks.Add(conveyor);
    }
  }
}
