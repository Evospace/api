#pragma once

#include "CoreMinimal.h"
#include "BlockLogicStore.h"
#include "Qr/CoordinameMinimal.h"
#include "Qr/Prototype.h"
#include "Qr/Vector.h"

#include "Public/MapChangeSet.h"
#include "Public/SurfaceDefinition.h"

#include "DimensionRuntime.generated.h"

class ADimension;
class FJsonObject;
class UColumnStreamingManager;
class UBlockLogic;
class UBlockNetwork;
class UConveyorNetwork;
class UDroneManager;
class ULogicContext;
class URailNetwork;
class URailwayManager;
class UResourceNetworkManager;
class UStaticBlock;
class UGameInstance;
class UGameSessionSubsystem;
class UWorld;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnRuntimeLogicAdded, const FQrVector3i &, UBlockLogic *);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnRuntimeLogicRemoved, const FQrVector3i &, UBlockLogic *);

/** What to do with sector-cell writes when the target column is not streamed in. */
enum class ECellWritePolicy : uint8 {
  /** Fail/skip like direct gameplay always did — building requires loaded terrain. */
  RequireLoaded,
  /** Apply the sim part now and queue the cell write for the column cell-edit job (remote edits, undo/redo). */
  QueueIfUnloaded,
};

UCLASS()
class EVOSPACE_API UDimensionRuntime : public UInstance {
  GENERATED_BODY()

  public:
  void InitializeForSurface(const FString &InSurfaceFolderName);
  void BindDimension(class ADimension *InDimension);
  void UnbindDimension(const ADimension *InDimension);
  void Shutdown();

  /** Clears all block logic and manager state. Recreates managers when InPresentation is set (after bind). */
  void ResetSimulationState(class ADimension *InPresentation);

  void ApplyLogicInput(UBlockLogic *Target, const ULogicContext *Context);

  UBlockLogic *SetBlockLogic(FQrVector3i Pos, UBlockLogic *Logic);
  UBlockLogic *GetBlockLogic(FQrVector3i BlockPos) const;

  /** Spawn a block with all its part cells: logic always, sector cells and renderables per Policy.
   * ConfigureRoot (settings/paint restore) runs after SpawnBlockPostprocess, before BlockBeginPlay. */
  UBlockLogic *SpawnBlockFull(const FQrVector3i &BlockPos, const UStaticBlock *StaticBlock, const FQuat &Rotation,
                              ECellWritePolicy Policy = ECellWritePolicy::RequireLoaded,
                              const TFunction<void(UBlockLogic *)> &ConfigureRoot = nullptr);
  UBlockLogic *SpawnBlockFullIdentity(const FQrVector3i &BlockPos, const UStaticBlock *StaticBlock);

  /** Remove a block with all its part cells (or clear a static-only cell). Returns the removed root logic.
   * Logic always leaves the runtime; cell clears follow Policy. */
  UBlockLogic *DestroyBlockFull(const FQrVector3i &BlockPos, ECellWritePolicy Policy = ECellWritePolicy::RequireLoaded);

  /** Block cells involved in a break at BlockPos (multi-cell-aware). */
  TArray<FQrVector3i> GatherBreakFootprint(const FQrVector3i &BlockPos) const;

  /** Sample current sector + logic state for undo/patch payloads. Logic JSON is captured at the logic root cell only. */
  FMapCellState CaptureCellState(const FQrVector3i &Pos) const;

  /** Apply snapshots produced by CaptureCellState in a safe order (clear, logic roots, static-only fills).
   * Logic applies to the sim immediately; cells of unloaded columns queue for the column cell-edit job. */
  void ApplyCellSnapshots(const TArray<FMapCellState> &Targets);

  /** Apply Before (undo) or After (redo) from a baked change record. */
  void ApplyChangeSet(const FMapChangeSet &Change, EMapChangeDirection Direction);

  /** Run ApplyBody once and return only cells whose state changed vs captured Before snapshots. */
  FMapChangeSet MakeEditChangeSet(const TArray<FQrVector3i> &Positions, TFunctionRef<void()> ApplyBody);

  const FString &GetSurfaceFolderName() const { return SurfaceFolderName; }

  /** Seed base for per-block deterministic RNG: derived from the world seed and this
   * surface, independent of block order. Resolved lazily (the seed string may be set
   * after the runtime is created); spawns are rare so the lookup cost is irrelevant. */
  uint64 GetBlockRngSeedBase() const;
  /** World/surface authoring data synced from ADimension during BindDimension; gameplay must not depend on presentation. */
  USurfaceDefinition *GetSurfaceDefinition() const { return SurfaceDefinition; }

  const TMap<FQrVector3i, UBlockLogic *> &GetLogics() const { return DimensionLogics; }

  void RemoveActorOrRenderableForAllTicked();
  void ResetTickStore();

  UBlockNetwork *GetNewNetwork();
  UDroneManager *GetDroneManager();
  URailNetwork *GetRailNetwork();
  URailwayManager *GetRailwayManager();
  UConveyorNetwork *GetNewConveyorNetwork();
  void KillConveyorNetworkDeferred(UConveyorNetwork *Network);
  void KillNetworkDeferred(UBlockNetwork *Network);

  void CaptureNetworkStorageSnapshots();
  void ClearNetworkStorageSnapshots();
  void SerializeRuntimeManagers(TSharedPtr<FJsonObject> JsonLogic) const;
  void DeserializeRuntimeManagers(TSharedPtr<FJsonObject> JsonObject);

  void BeginDeferredResourceNetworkLoad();
  void FinishDeferredResourceNetworkLoad(TSharedPtr<FJsonObject> JsonLogic);
  void CancelDeferredResourceNetworkLoad();
  bool IsResourceNetworkRebuildDeferred() const { return bDeferResourceNetworkRebuild; }

  int32 TickRuntime(UWorld *World, UGameInstance *GameInstance, float DeltaTime, int32 TickRate, bool TickBlocksEnabled);
  void TickVisual(float DeltaTime);

  /**
   * SimBench: run Count simulation substeps back-to-back at max throughput, bypassing
   * the DeltaTime accumulator (fixed step 1/TickRate for railways). Returns ticks run.
   * Metrics must be enabled by the caller (BeginSession) for per-tick samples to be recorded.
   */
  int64 RunBenchTicks(int32 Count, int32 TickRate, UGameInstance *GameInstance);

  /** While set, the normal ADimension::TickBlocks ticker stands down: SimBench drives the sim. */
  static void SetBenchOwnsSim(bool bOwns);
  static bool IsBenchOwningSim();

  /** Desync hash: fill section hashes of the deterministic sim state (blocks/networks/trains).
   * Diagnostic tool driven by FSimStateHash, never on the hot path unless the gate is on. */
  void ComputeSimStateHash(struct FSimStateHashRecord &Out) const;

  int32 GetManagedResourceNetworkCount() const;
  int32 GetActiveConveyorNetworkCount() const;

  FOnRuntimeLogicAdded OnRuntimeLogicAdded;
  FOnRuntimeLogicRemoved OnRuntimeLogicRemoved;

  private:
  void TickConveyorNetworks();

  /** One simulation substep: all sim systems + optional block logic, then commit a metrics sample. */
  void RunSimSubstep(int32 TickRate, UGameSessionSubsystem *GameSession, bool bTickBlocksEnabled);

  UBlockLogic *SpawnBlockCell(const FQrVector3i &BlockPos, UBlockLogic *Parent, const UStaticBlock *StaticBlock,
                              const FQuat &Rotation, ECellWritePolicy Policy,
                              const TFunction<void(UBlockLogic *)> &ConfigureRoot);

  /** Restore logic + parts from a SaveSettings/LoadSettings snapshot (+ PaintMaterial). Root cell only. */
  void RestoreBlockFromSnapshot(const FMapCellState &S);

  /** Write one sector cell: direct when the column is streamed, queued per Policy otherwise. */
  bool WriteCell(const FQrVector3i &Pos, const UStaticBlock *Block, const BlockDensity &Density, ECellWritePolicy Policy);

  UPROPERTY(VisibleAnywhere)
  FString SurfaceFolderName;

  /** Bound presentation actor; renderable/actor spawn goes through it. */
  UPROPERTY()
  ADimension *Presentation = nullptr;

  /** Bound presentation's column manager; sector cell reads/writes go through it. */
  UPROPERTY()
  UColumnStreamingManager *ColumnManager = nullptr;

  UPROPERTY()
  USurfaceDefinition *SurfaceDefinition = nullptr;

  UPROPERTY()
  TMap<FQrVector3i, UBlockLogic *> DimensionLogics;

  FBlockLogicStore DimensionLogicsTick;

  UPROPERTY()
  UResourceNetworkManager *ResourceNetworkManager = nullptr;

  UPROPERTY()
  UDroneManager *DroneManager = nullptr;

  UPROPERTY()
  URailNetwork *RailNetwork = nullptr;

  UPROPERTY()
  URailwayManager *RailwayManager = nullptr;

  UPROPERTY()
  TArray<UConveyorNetwork *> ConveyorNetworks;

  UPROPERTY()
  TArray<UConveyorNetwork *> ConveyorNetworksToKill;

  int64 TickAccumulatorMicros = 0;

  bool bDeferResourceNetworkRebuild = false;
};
