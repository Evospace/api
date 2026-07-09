// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "EvoRingBuffer.h"
#include "Evospace/ThreadWorker.h"
#include "Qr/Vector.h"
#include "Evospace/Props/DimensionPropComponent.h"
#include "Evospace/Props/StaticIndexedHierarchicalInstancedStaticMeshComponent.h"
#include "Evospace/World/Column.h"
#include "Evospace/World/ColumnSaveRunner.h"
#include "Evospace/World/SectorCompiler.h"
#include "Public/StaticBlock.h"
#include "Public/LazyGameSessionData.h"
#include "Public/MapChangeSet.h"
#include <Containers/CircularBuffer.h>
#include <Engine/EngineTypes.h>
#include <Math/Vector.h>
#include <Containers/Ticker.h>
#include "Evospace/Props/ItemInstancingComponent.h"

#include "Dimension.generated.h"

class UStaticCover;
class UGraphStorage;
class UStaticIndexedItemInstancedStaticMeshComponent;
class AWorldFeaturesManager;
namespace evo {
class LegacyLuaState;
}
class UBlockNetwork;
class UStaticItem;
class ASector;
class USectorSaver;
class AMainPlayerController;
class UItemInstancingComponent;
class UBlockLogic;
class UStaticProp;
class UDimensionPropComponent;
class UStaticIndexedHierarchicalInstancedStaticMeshComponent;
class USectorCompiler;
class APlayerController;
class UDroneManager;
class URailNetwork;
class URailwayManager;
class UColumnStreamingManager;
class UDimensionRuntime;
class UDimensionLoadWidget;
class USurfaceDefinition;
class UGameSessionData;
class UInstancedStaticMeshComponent;
class ULogicContext;
class FColumnSaveRunner;

UCLASS()
class ADimension : public AActor {
  public:
  GENERATED_BODY()
  static void lua_reg(lua_State *L) {
    luabridge::getGlobalNamespace(L)
      .deriveClass<ADimension, AActor>("Dimension") //@class Dimension : Actor
      // direct:
      //---Spawn a block at the given position
      //---@param bpos Vec3i Block position
      //---@param quat Quat Rotation
      //---@param staticBlock StaticBlock Block type to spawn
      //---@return BlockLogic The spawned block logic instance
      //function Dimension:spawn_block(bpos, quat, staticBlock) end
      .addFunction("spawn_block", &ADimension::LuaSpawnBlockLogic)
      // direct:
      //---Spawn a block with full identity at the given position
      //---@param bpos Vec3i Block position
      //---@param staticBlock StaticBlock Block type to spawn with identity rotation
      //---@return BlockLogic The spawned block logic instance
      // function Dimension:spawn_block_identity(bpos, staticBlock) end
      .addFunction("spawn_block_identity", &ADimension::SpawnLogicFullIdentity)
      // direct:
      //---Set a block cell at the given position
      //---@param bpos Vec3i Block position
      //---@param cl StaticBlock|nil Block type to set
      // function Dimension:set_cell(bpos, cl) end
      .addFunction("set_cell", &ADimension::LuaSetBlockCell)
      // direct:
      //---Clear all props at the given position
      //---@param bpos Vec3i Block position
      //function Dimension:clear_props(bpos) end
      .addFunction("clear_props", &ADimension::LuaCleanProps)
      // direct:
      //---Sample terrain height at XY in world-cell coordinates
      //---@param x number X coordinate (world cells)
      //---@param y number Y coordinate (world cells)
      //---@return number Height in cells
      //function Dimension:sample_height(x, y) end
      .addFunction("sample_height", &ADimension::LuaSampleHeight)
      .endClass();
  }

  ADimension();

  UFUNCTION(BlueprintCallable)
  void BeginTeleport(const FVector &pos);

  UFUNCTION(BlueprintCallable)
  float GetTps() const;

  UFUNCTION(BlueprintCallable)
  float GetTickDuration() const;

  public:
  // Core Functions

  USectorProxy *GetSector(const Vec3i &spos);
  const USectorProxy *GetSector(const Vec3i &spos) const;

  USectorProxy *FindBlockCell(const Vec3i &pos, IndexType &index);
  const USectorProxy *FindBlockCell(const Vec3i &pos, IndexType &index) const;

  UFUNCTION(BlueprintCallable)
  void SaveDimentionFolder(bool backup = false, bool bCapturePreview = false);

  /** Flush the temp working copy for a network guest join. Returns false if column IO is still busy. */
  bool FlushWorkingCopyForNetworkJoin();

  USectorSaver *GetSectorSaver();

  void LoadDimentionFolder();
  void LuaSetBlockCell(const Vec3i &bpos, const UStaticBlock *cl);
  void LuaCleanProps(const Vec3i &bpos);

  // Lua: dim:sample_height(x, y)
  float LuaSampleHeight(float x, float y) const;

  virtual void BeginPlay() override;

  UBlockLogic *LuaSpawnBlockLogic(const Vec3i &bpos, const FQuat &block_q, const UStaticBlock *cl);

  RItemInstancingHandle AddItemInstance(const UStaticItem *prop, const FTransform &t) const;

  void RemoveInstance(const UStaticItem *prop, int32 index);

  UBlockLogic *SetBlockLogic(Vec3i pos, UBlockLogic *logic);
  UBlockLogic *DestroyLogicFull(const Vec3i &pos);
  UBlockLogic *SpawnLogicFullIdentity(const Vec3i &bpos, const UStaticBlock *staticObject);
  UBlockLogic *SpawnLogicFull(const Vec3i &bpos, const UStaticBlock *staticObject, const FQuat &rot);

  /** Block cells involved in a break at wbpos (multi-cell-aware). */
  TArray<Vec3i> GatherBreakFootprint(Vec3i wbpos);

  /** Sample current sector + logic state for undo/patch payloads. Logic JSON is captured at the logic root cell only. */
  FMapCellState CaptureMapCellState(Vec3i Pos);

  /** Apply snapshots produced by CaptureMapCellState in a safe order (clear, logic roots, static-only fills). */
  void ApplyMapCellSnapshots(const TArray<FMapCellState> &Targets);

  /** Apply Before (undo) or After (redo) from a baked change record. */
  void ApplyMapChangeSet(const FMapChangeSet &Change, EMapChangeDirection Direction);

  /** Run ApplyBody once and return only cells whose state changed vs captured Before snapshots (unique Positions list). */
  FMapChangeSet MakeEditChangeSet(const TArray<Vec3i> &Positions, TFunctionRef<void()> ApplyBody);

  /** Remove logic from FinishSpawning queue (must run if actor/render teardown happens before DeferredActorSpawn sees the entry). */
  void DiscardPendingDeferredRenderable(UBlockLogic *Logic);

  /** Column streaming, caching and the pending cell-edit machinery. */
  UColumnStreamingManager *GetColumnManager() const { return ColumnManager; }

  UFUNCTION(BlueprintCallable)
  void InitializeSurface(USurfaceDefinition *surfaceDefinition, bool bDestroyPreviousOnSwitch = true);
  UDimensionRuntime *EnsureSimulationRuntimeCreated();
  bool BindToSimulationRuntime();
  /** Destroys streamed columns and clears dimension presentation, then clears ADimension::Runtime. */
  void UnbindFromSimulationRuntime();

  /** USimulationSurfaceSubsystem::ApplySurfaceLifecycle(ResetBeforeLoad) — call immediately before LoadDimentionFolder. */
  void PrepareSimulationRuntimeForLoad();

  /** ADimension::Runtime must be set for SectorArea to advance LoadedPos/Radius and for LoadColumn to run. */
  bool IsSimulationRuntimeBound() const { return Runtime != nullptr; }

  /** Current simulation runtime without creating one; null when unbound (e.g. preview/menu). */
  UDimensionRuntime *GetSimulationRuntime() const { return Runtime; }

  virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

  virtual void Tick(float DeltaTime) override;
  void AddBlockBuildCover(const TTuple<FQrVector3i, UBlockLogic *> &log) const;

  bool mBuildModeBoxes = false;

  bool mAccessorsPlanes = false;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  USurfaceDefinition *SurfaceDefinition;

  /** Folder name under the save directory for this playable surface (e.g. Temperate); used for SurfaceDefinition.json, logic bin, weather. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Surface")
  FString SurfaceFolderName = TEXT("Temperate");

  UPROPERTY()
  TArray<UBlockLogic *> RenderBlocksDeferred;

  /** Maximum time budget (in milliseconds) per frame for deferred actor FinishSpawning. */
  UPROPERTY(EditAnywhere, Category = "Performance")
  float DeferredActorSpawnTimeMs = 3.0f;

  UFUNCTION(BlueprintCallable)
  void ActorBuildModeToggle(bool enabled);

  UFUNCTION(BlueprintCallable)
  void Pause();

  UFUNCTION(BlueprintCallable)
  void Unpause();

  UBlockNetwork *GetNewNetwork();
  UDroneManager *GetDroneManager();
  URailNetwork *GetRailNetwork();
  URailwayManager *GetRailwayManager();
  class UConveyorNetwork *GetNewConveyorNetwork();
  void KillConveyorNetworkDeffered(class UConveyorNetwork *network);

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
  UGraphStorage *ResourceGraph;

  UPROPERTY(VisibleAnywhere)
  UDimensionPropComponent *DimensionPropComponent;

  UPROPERTY()
  UStaticIndexedHierarchicalInstancedStaticMeshComponent *BuildingModCover;

  UPROPERTY()
  TMap<const UStaticCover *, UStaticIndexedHierarchicalInstancedStaticMeshComponent *> mAccessorCover;

  UPROPERTY(EditAnywhere)
  AWorldFeaturesManager *FeatureManager;

  protected:
  // Core Tick

  float HalfSecond = 0.f;
  int32 Ticks = 0;
  int32 HalfTps = 0;
  EvoRingBuffer<float> TpsBuffer = EvoRingBuffer<float>(16, 0);
  EvoRingBuffer<float> CyclesBuffer = EvoRingBuffer<float>(16, 0);

  bool Paused = false;

  UPROPERTY(EditAnywhere)
  bool mTicking = true;

  FTSTicker::FDelegateHandle TickDelegateHandle;

  private:
  UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
  bool bSectorGenegateOnly = true;

  void TeleportProcess();
  void LoadingProcess();

  float mPlayerTeleportDelay = 1.0;
  bool mPlayerTeleported = false;
  bool mFirstLoading = true;

  UPROPERTY(VisibleAnywhere)
  USectorCompiler *mSectorCompiler;

  UPROPERTY(VisibleAnywhere)
  USectorSaver *mSectorSaver;

  UPROPERTY(VisibleAnywhere)
  APlayerController *mController;

  UPROPERTY(VisibleAnywhere)
  TSubclassOf<UDimensionLoadWidget> mLoadWidgetClass;

  UPROPERTY(VisibleAnywhere)
  UDimensionLoadWidget *mLoadWidget;

  UPROPERTY()
  UDimensionRuntime *Runtime = nullptr;

  UPROPERTY()
  bool BuildModeCoverDirty = false;

  void HandleRuntimeLogicAdded(const FQrVector3i &Pos, UBlockLogic *Logic);
  void HandleRuntimeLogicRemoved(const FQrVector3i &Pos, UBlockLogic *Logic);

  private:
  // Columns

  friend class AColumn;
  friend class FColumnSaveRunner;
  friend class USectorArea;

  void SaveDirtyColumnsForSnapshot();

  UPROPERTY()
  UColumnStreamingManager *ColumnManager = nullptr;

  public:
  void LoadColumn(const Vec3i &pos);
  void UnloadColumn(const Vec3i &pos);

  bool IsCanCreateColumn() const;
  bool IsCanRemoveColumn() const;
  bool IsCanSaveColumn(const AColumn &tall) const;

  private:
  void DeferredActorSpawn();
  void TickProcess(float DeltaTime);
  bool TickBlocks(float DeltaTime);

  bool IsSnapshotBarrierReady() const;

  void SaveDimentionFolderImpl(bool backup, bool bCapturePreview = false);
  void TickPendingFullSave();

  bool mPendingFullSave = false;
  bool mPendingFullSaveBackup = false;
  bool mPendingFullSaveCapturePreview = false;

  void TearDownSurfaceColumnsAndPresentation();
  void InvalidateAllPlayerSectorStreamingAnchors();

  public:
  UBlockLogic *GetBlockLogic(Vec3i bpos);

  void KillNetworkDeffered(UBlockNetwork *mNetwork);

  private:
  /** Last SurfaceFolderName for which InitializeSurface completed; used to detect surface switches and orphaned columns. */
  UPROPERTY()
  FString ActivePresentationSurfaceFolder;

  UPROPERTY(VisibleAnywhere)
  FLazyGameSession GameSessionCache;
};
