#include "SectorProxy.h"

#include "EngineUtils.h"
#include "Evospace/World/Sector.h"
#include "Evospace/World/SectorCompiler.h"
#include "Public/Dimension.h"

#include "Public/StaticBlock.h"
bool ISectorProxy::SetDataForCompiler(SectorCompilerData &data){
  SetDirty(false);

  for (int i = 0; i < Vec3i(3, 3, 3).Capacity(); ++i) {
    auto pos = cs::IndexToArea(i, Vec3i(-1, -1, -1), Vec3i(1, 1, 1));
    data.SetSector(pos, GetAdjacentSectors().Element(pos));
  }

  return true;
}

USectorProxyHolder::USectorProxyHolder() {
  StaticBlocks.Init({}, gSectorSize.Capacity());
}

void USectorProxyHolder::SpawnColumnCallback(const FSectorData &data) {
  Dirty = true;
  SectorColdData = data;
}

void USectorProxyHolder::GetSectorDataHot(FSectorData &data) {
  SaveChanged = false;
  data = SectorColdData;
}

void USectorProxyHolder::GetSectorDataCold(FSectorData &data) {
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

void USectorProxyHolder::Destroy(){
    if(IsValid(sector)) {
        sector->Destroy();
        sector = nullptr;
    }
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

bool USectorProxyHolder::ApplyDataFromCompiler(ADimension * dim, UTesselator::Data &&data, int32 lod, TFunction<void()> callback){
  if (data.Num() > 0) {
    FTransform transform;
    transform.SetLocation(cs::WBtoWd(GetPivotPos()));
    auto actor = dim->GetWorld()->SpawnActor<ASector>(dim->mSectorClass, transform);
    actor->SetProxyData(GetPivotPos(), owner);
    actor->LoadSector(*column);
    actor->ApplyDataFromCompiler(dim, MoveTemp(data), lod, callback);
    this->sector = actor;
  } else {
    callback();
  }
  return true;
}