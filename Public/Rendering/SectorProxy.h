#pragma once

#include "CoreMinimal.h"
#include "Evospace/World/BlockCell.h"
#include "StaticLogger.h"
#include "Evospace/World/SectorTools.h"
#include "Evospace/World/Tesselator.h"
#include "SectorProxy.generated.h"

class URealtimeMeshComponent;
class ADimension;
class AColumn;
class USectorPropComponent;
class UBlockLogic;
class ABlockActor;

UCLASS()
class USectorProxy : public UObject {
  GENERATED_BODY()
  public:
  USectorProxy();

  bool SetDataForCompiler(SectorCompilerData &data);

  void SpawnColumn(const FSectorData &data);

  // Visible sector can be changed, so we need to fill data here from runtime components
  void GetSectorDataHot(FSectorData &data);

  // Invisible sector can't be changed so data remains the same
  void GetSectorDataCold(FSectorData &data);

  void Destroy();

  bool GetDirty() const { return Dirty; }
  void SetDirty(bool value) { Dirty = value; }

  void SetDirty(IndexType index);

  FVector3i GetPivotPos() const { return PivotPos; }
  void SetProxyData(FVector3i value, AColumn *actor) {
    PivotPos = value;
    owner = actor;
  }

  virtual const AjacentSectors &GetAdjacentSectors() const { return ajacentSectors; }
  virtual AjacentSectors &GetAdjacentSectors() { return ajacentSectors; }

  void SetStaticBlock(IndexType index, const UStaticBlock *value);
  const UStaticBlock *GetStaticBlock(IndexType index);

  void SetBlockDensity(IndexType index, BlockDensity density);
  const BlockDensity &GetBlockDensity(IndexType index) const;

  const TArray<FStaticBlockInfo> &GetStaticBlocks() const { return StaticBlocks; }

  void LoadSector(const AColumn &c);

  void UnloadSector();

  virtual bool ApplyDataFromCompiler(ADimension *dim, UTesselator::Data &&data, int32 lod, TFunction<void()> callback);

  virtual USectorPropComponent *GetInstancingComponent() const;

  virtual void ClearBlockProps(const FVector3i &bpos, bool doDrop);

  AColumn *GetColumn() const { return owner; }

  UPROPERTY(VisibleAnywhere)
  AColumn *owner = nullptr;

  UPROPERTY(VisibleAnywhere)
  FVector3i PivotPos = {};

  UPROPERTY(VisibleAnywhere)
  bool Dirty = false;

  FSectorData SectorColdData;

  AjacentSectors ajacentSectors;

  TArray<FStaticBlockInfo> StaticBlocks;

  UPROPERTY()
  ASector *sector = nullptr;

  UPROPERTY()
  bool IsSectionGroupCreated = false;

  UPROPERTY()
  URealtimeMeshComponent *rmc = nullptr;
};