// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Qr/CoordinameMinimal.h"

#include "GrassStreamingSubsystem.generated.h"

class AActor;
class UStaticProp;
class UStaticIndexedHierarchicalInstancedStaticMeshComponent;

UCLASS()
class UGrassStreamingSubsystem : public UTickableWorldSubsystem {
  GENERATED_BODY()

  public:
  using FHism = UStaticIndexedHierarchicalInstancedStaticMeshComponent;

  static UGrassStreamingSubsystem *Get(const UObject *worldContext);

  void RegisterSectorProps(const FIntPoint &sectorPos, const UStaticProp *prop, const TArray<FTransform3f> &transforms);
  void UnregisterSector(const FIntPoint &sectorPos);

  void AppendSurvivors(const FIntPoint &sectorPos, TMap<const UStaticProp *, TArray<FTransform3f>> &out) const;

  bool DestroyInBlock(const Vec3i &bpos, bool only_small);
  bool DestroyInstance(FHism *comp, int32 staticIndex);
  bool DestroyInstanceAt(const UStaticProp *prop, const FVector &loc);

  TArray<const UStaticProp *> Get(const Vec3i &bpos) const;
  const UStaticProp *LookForProp(const FHism *comp) const;

  int32 TileSize = 2;
  int32 RingRadius = 2;
  int32 RingHysteresis = 1;
  int32 MaxInstancesPerFrame = 2048;

  virtual void Tick(float DeltaTime) override;
  virtual TStatId GetStatId() const override;
  virtual void Deinitialize() override;

  private:
  struct FSectorSurvivors {
    TMap<const UStaticProp *, TArray<FTransform3f>> props;
  };

  struct FTile {
    bool materialized = false;
    bool dirty = false;
    TMap<const UStaticProp *, FHism *> hisms;
  };

  TMap<FIntPoint, FSectorSurvivors> mSectors;
  TMap<FIntPoint, FTile> mTiles;
  TMap<const FHism *, const UStaticProp *> mHismToProp;

  TWeakObjectPtr<AActor> mHolder;

  FIntPoint mLastPlayerTile = FIntPoint(MAX_int32, MAX_int32);
  bool mHasLastPlayer = false;
  mutable FVector mLastCenterLoc = FVector::ZeroVector;

  FIntPoint SectorToTile(const FIntPoint &sectorPos) const;
  static FIntPoint BlockToSector(const Vec3i &bpos);
  bool GetPlayerSector(FIntPoint &outSector) const;

  AActor *EnsureHolder();
  FHism *MakeHism(const UStaticProp *prop);

  void MarkSectorTileDirty(const FIntPoint &sectorPos);
  void RemoveLiveInstancesInBlock(const FIntPoint &sectorPos, const Vec3i &bpos);
  void RemoveLiveInstanceAt(const FIntPoint &sectorPos, const UStaticProp *prop, const FVector &loc);
  void RebuildTile(const FIntPoint &tileCoord, FTile &tile, int32 &budget);
  void DestroyTile(const FIntPoint &tileCoord, FTile &tile);
  void ClearTileHisms(FTile &tile);
};
