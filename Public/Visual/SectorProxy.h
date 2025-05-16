#pragma once

#include "CoreMinimal.h"
#include "Evospace/World/BlockCell.h"
#include "Evospace/World/SectorTools.h"
#include "StaticLogger.h"
#include "Evospace/World/Tesselator.h"
#include "SectorProxy.generated.h"

class AColumn;
class USectorPropComponent;
class UBlockLogic;
class ABlockActor;

UINTERFACE()
class USectorProxy : public UInterface {
  GENERATED_BODY()
};

class ISectorProxy {
  GENERATED_BODY()
  public:
  using RenderableVariant = std::variant<bool, ABlockActor *>;

  virtual void SpawnColumnCallback(const FSectorData &data) = 0;

  // Visible sector can be changed, so we need to fill data here from runtime components
  virtual void GetSectorDataHot(FSectorData &data) = 0;

  // Invisible sector can't be changed so data remains the same
  virtual void GetSectorDataCold(FSectorData &data) = 0;

  virtual void PromoteToSector() = 0;

  virtual bool GetDirty() const = 0;
  virtual void SetDirty(bool value) = 0;

  virtual void LoadSector(const AColumn &column) = 0;
  virtual void UnloadSector() = 0;

  virtual FVector3i GetPivotPos() const = 0;
  virtual void SetProxyData(FVector3i value, AActor *actor) = 0;

  virtual const AjacentSectors &GetAdjacentSectors() const = 0;
  virtual AjacentSectors &GetAdjacentSectors() = 0;

  virtual void SetRenderable(IndexType index, RenderableVariant value, UBlockLogic *logic) {}

  virtual const TArray<FStaticBlockInfo> &GetStaticBlocks() const = 0;

  virtual void SetStaticBlock(IndexType index, const UStaticBlock *value) = 0;
  virtual const UStaticBlock *GetStaticBlock(IndexType index) = 0;

  virtual void SetBlockDecity(IndexType index, BlockDensity density) = 0;
  virtual const BlockDensity &GetBlockDesity(IndexType index) const = 0;

  virtual bool ApplyDataFromCompiler(ADimension * dim, UTesselator::Data &&data, int32 lod, TFunction<void()> callback) = 0;
  
  bool SetDataForCompiler(SectorCompilerData &data);

  virtual void ClearBlockProps(IndexType index, bool doDrop = true) {}

  virtual void Destroy() = 0;

  virtual bool IsSaveChanged() const { return false; }

  virtual FRotator GetActorRotation() { return {}; }
  virtual FVector GetActorLocation() { return {}; }

  virtual void SetActorHiddenInGame(bool v) {}

  virtual bool GetSectionGroupCreated() const { return false; }

  virtual USectorPropComponent *GetInstancingComponent() const { return nullptr; }
};

UCLASS()
class USectorProxyHolder : public UObject, public ISectorProxy {
  GENERATED_BODY()
  public:

  USectorProxyHolder();
  
  virtual void SpawnColumnCallback(const FSectorData &data) override;

  // Visible sector can be changed, so we need to fill data here from runtime components
  virtual void GetSectorDataHot(FSectorData &data) override;

  // Invisible sector can't be changed so data remains the same
  virtual void GetSectorDataCold(FSectorData &data) override;

  virtual void PromoteToSector() override;

  virtual void Destroy() override ;

  virtual bool GetDirty() const override { return Dirty; }
  virtual void SetDirty(bool value) override { Dirty = value; }

  virtual FVector3i GetPivotPos() const override { return PivotPos; }
  virtual void SetProxyData(FVector3i value, AActor *actor) override {
    PivotPos = value;
    owner = actor;
  }

  virtual const AjacentSectors &GetAdjacentSectors() const override { return ajacentSectors; }
  virtual AjacentSectors &GetAdjacentSectors() override { return ajacentSectors; }

  void SetStaticBlock(IndexType index, const UStaticBlock *value);
  const UStaticBlock *GetStaticBlock(IndexType index);

  virtual void SetBlockDecity(IndexType index, BlockDensity density) override;
  virtual const BlockDensity &GetBlockDesity(IndexType index) const override;

  virtual const TArray<FStaticBlockInfo> &GetStaticBlocks() const override { return StaticBlocks; }

  virtual void LoadSector(const AColumn &c) override { StaticBlocks = SectorColdData.mStaticBlocks; column = &c; }

  virtual void UnloadSector() override {
    SectorColdData = {};
    SectorColdData.mStaticBlocks = StaticBlocks;
  }

  virtual bool ApplyDataFromCompiler(ADimension * dim, UTesselator::Data &&data, int32 lod, TFunction<void()> callback) override;
  
  UPROPERTY(VisibleAnywhere)
  AActor *owner = nullptr;

  UPROPERTY(VisibleAnywhere)
  FVector3i PivotPos = {};

  UPROPERTY(VisibleAnywhere)
  bool SaveChanged = false;

  UPROPERTY(VisibleAnywhere)
  bool Dirty = false;

  FSectorData SectorColdData;

  AjacentSectors ajacentSectors;

  TArray<FStaticBlockInfo> StaticBlocks;

  UPROPERTY()
  ASector * sector = nullptr;

  UPROPERTY()
  const AColumn * column = nullptr;
};