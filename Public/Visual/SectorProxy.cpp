#include "SectorProxy.h"

#include "Public/StaticBlock.h"
void USectorProxyHolder::SpawnColumnCallback(const FSectorData &data) {
  Dirty = true;
  SectorColdData = data;
}

void USectorProxyHolder::SetDataForLoaderCold(FSectorData &data) {
  SaveChanged = false;
  data = SectorColdData;
}

void USectorProxyHolder::PromoteToSector() {
  // if (ensure(sector)) {
  //   sector->mMyColumn = column;
  //   sector->mPivotPos = spos * gSectorSize;
  //   sector->SetOwner(this);
  //   column->sectors.Add(sector);
  //   column->compiling.Add(false);
  //   sector->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));
  // }
}
void USectorProxyHolder::SetStaticBlock(IndexType index, const UStaticBlock *value) {
  StaticBlocks[index].block = value;

  if (!value || value->Tesselator) {
    SetDirty(index);
  }
}
const UStaticBlock *USectorProxyHolder::GetStaticBlock(IndexType index) {
  return StaticBlocks[index].block;
}

void USectorProxyHolder::SetBlockDecity(IndexType index, BlockDensity density) {
  SetDirty(index);
  StaticBlocks[index].dencity = density;
}

const BlockDensity &USectorProxyHolder::GetBlockDesity(IndexType index) const {
  return StaticBlocks[index].dencity;
}