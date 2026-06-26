// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Qr/CoordinameMinimal.h"

#include "GrassStreamingSubsystem.generated.h"

class AActor;
class UStaticProp;
class UStaticIndexedHierarchicalInstancedStaticMeshComponent;

/**
 * Player-centric streaming of small mass props (grass, flowers, pebbles).
 *
 * Columns still generate placement and hand the streamed subset here via RegisterSectorProps
 * (instead of building per-column HISM). The subsystem keeps the lightweight survivor lists for
 * all loaded columns, but materializes HISM only inside a tile ring around the player, batching
 * one HISM per prop type per coarse tile. Removal (mining / breaking brush) edits the survivors
 * and rebuilds the affected tile; survivors are written back into the column save in
 * AppendSurvivors so removal persists.
 */
UCLASS()
class UGrassStreamingSubsystem : public UTickableWorldSubsystem {
  GENERATED_BODY()

  public:
  using FHism = UStaticIndexedHierarchicalInstancedStaticMeshComponent;

  static UGrassStreamingSubsystem *Get(const UObject *worldContext);

  // --- Column lifecycle (called from sector materialization / unload) ---
  void RegisterSectorProps(const FIntPoint &sectorPos, const UStaticProp *prop, const TArray<FTransform3f> &transforms);
  void UnregisterSector(const FIntPoint &sectorPos);

  // --- Save: append surviving streamed props of a column into its save data ---
  void AppendSurvivors(const FIntPoint &sectorPos, TMap<const UStaticProp *, TArray<FTransform3f>> &out) const;

  // --- Removal ---
  /** Remove streamed props at a world block position (only_small ignored: streamed props are all small). */
  void DestroyInBlock(const Vec3i &bpos, bool only_small);
  /** Remove the streamed instance addressed by a HISM + its static index (breaking brush). */
  bool DestroyInstance(FHism *comp, int32 staticIndex);

  // --- Queries ---
  TArray<const UStaticProp *> Get(const Vec3i &bpos) const;
  const UStaticProp *LookForProp(const FHism *comp) const;

  // --- Tunables (overridable live via evo.GrassStream.* console variables) ---
  /** Tile edge in sectors (1 sector = gFlatSectorSize blocks). */
  int32 TileSize = 2;
  /** Materialization radius in tiles around the player view. */
  int32 RingRadius = 2;
  /** Extra tiles kept materialized before dematerializing (hysteresis). */
  int32 RingHysteresis = 1;
  /** Soft budget of instances built per tick. */
  int32 MaxInstancesPerFrame = 2048;

  // --- UTickableWorldSubsystem ---
  virtual void Tick(float DeltaTime) override;
  virtual TStatId GetStatId() const override;
  virtual void Deinitialize() override;

  private:
  struct FSectorSurvivors {
    // Streamed props surviving in this column, keyed by prop type.
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

  // Holder actor kept at world origin; all streamed HISM attach to its root so that
  // component-relative instance transforms equal world transforms (mirrors USectorPropComponent).
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
  void RebuildTile(const FIntPoint &tileCoord, FTile &tile, int32 &budget);
  void DestroyTile(const FIntPoint &tileCoord, FTile &tile);
  void ClearTileHisms(FTile &tile);
};
