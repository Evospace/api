// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "BlockLogicStore.h"
#include "EvoRingBuffer.h"
#include "Evospace/ThreadWorker.h"
#include "Qr/Vector.h"
#include "Evospace/Props/DimensionPropComponent.h"
#include "Evospace/Props/StaticIndexedHierarchicalInstancedStaticMeshComponent.h"
#include "Evospace/World/Column.h"
#include "Evospace/World/SectorCompiler.h"
#include "Public/StaticBlock.h"
#include "Public/LazyGameSessionData.h"
#include <Containers/CircularBuffer.h>
#include <Engine/EngineTypes.h>
#include <Math/Vector.h>
#include <Containers/Ticker.h>
#include "Evospace/Props/ItemInstancingComponent.h"
#include <memory>

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
class UResourceNetworkManager;
class UDimensionLoadWidget;
class USurfaceDefinition;
class UGameSessionData;

USTRUCT()
struct EVOSPACE_API FStatictics {
  GENERATED_BODY()

  public:
};

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
      // function Dimension:spawn_block(bpos, quat, staticBlock) end
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
      // function Dimension:clear_props(bpos) end
      .addFunction("clear_props", &ADimension::LuaCleanProps)
      // direct:
      //---Sample terrain height at XY in world-cell coordinates
      //---@param x number X coordinate (world cells)
      //---@param y number Y coordinate (world cells)
      //---@return number Height in cells
      //.function Dimension:sample_height(x, y) end
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
  void SaveDimentionFolder(bool backup = false);

  USectorSaver *GetSectorSaver();

  void LoadDimentionFolder();
  void LuaSetBlockCell(const Vec3i &bpos, const UStaticBlock *cl);
  void LuaCleanProps(const Vec3i &bpos);

  // Lua: dim:sample_height(x, y)
  float LuaSampleHeight(float x, float y) const;

  virtual void BeginPlay() override;

  UBlockLogic *LuaSpawnBlockLogicDefault(const Vec3i &bpos, const UStaticBlock *cl);
  UBlockLogic *LuaSpawnBlockLogic(const Vec3i &bpos, const FQuat &block_q, const UStaticBlock *cl);

  RItemInstancingHandle AddItemInstance(const UStaticItem *prop, const FTransform &t) const;

  void RemoveInstance(const UStaticItem *prop, int32 index);

  UBlockLogic *SetBlockLogic(Vec3i pos, UBlockLogic *logic);
  UBlockLogic *DestroyLogicFull(const Vec3i &pos);
  UBlockLogic *SpawnLogicFullIdentity(const Vec3i &bpos, const UStaticBlock *staticObject);
  UBlockLogic *SpawnLogicFull(const Vec3i &bpos, const UStaticBlock *staticObject, const FQuat &rot);

  UBlockLogic *_SpawnLogicFull(const Vec3i &bpos, UBlockLogic *parent, const UStaticBlock *staticObject,
                               const FQuat &rot);

  UFUNCTION(BlueprintCallable)
  void InitializeSurface(USurfaceDefinition *surfaceDefinition);

  virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

  virtual void Tick(float DeltaTime) override;
  void AddBlockBuildCover(const TTuple<FVector3i, UBlockLogic *> &log) const;

  bool mBuildModeBoxes = false;

  bool mAccessorsPlanes = false;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  USurfaceDefinition *SurfaceDefinition;

  UPROPERTY()
  TArray<UBlockLogic *> RenderBlocksDeferred;

  UFUNCTION(BlueprintCallable)
  void ActorBuildModeToggle(bool enabled);

  UFUNCTION(BlueprintCallable)
  void Pause();

  UFUNCTION(BlueprintCallable)
  void Unpause();

  UBlockNetwork *GetNewNetwork();
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

  // UPROPERTY(EditAnywhere)
  // UBatchBlockLogicManager *BatchManager;

  protected:
  // Core Tick

  float HalfSecond = 0.f;
  float DeltaRemain = 0.f;
  int32 Ticks = 0;
  int32 HalfTps = 0;
  EvoRingBuffer<float> TpsBuffer = EvoRingBuffer<float>(16, 0);
  EvoRingBuffer<float> CyclesBuffer = EvoRingBuffer<float>(16, 0);

  // Integer accumulator in tick-micro units (deltaMicros * tickRate)
  int64 TickAccumulatorMicros = 0;

  bool Paused = false;

  UPROPERTY(EditAnywhere)
  bool mTicking = true;

  UPROPERTY(EditAnywhere)
  bool mNetworkTicking = true;

  FTSTicker::FDelegateHandle TickDelegateHandle;

  private:
  UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
  bool bSectorGenegateOnly = true;

  void TeleportProcess();
  void LoadingProcess();

  // Conveyor networks storage
  UPROPERTY()
  TArray<class UConveyorNetwork *> mConveyorNetworks;

  UPROPERTY()
  TArray<class UConveyorNetwork *> mConveyorNetworksToKill;

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
  TMap<FVector3i, UBlockLogic *> mDimensionLogics;

  UPROPERTY()
  FBlockLogicStore mDimensionLogicsTick;

  UPROPERTY(VisibleAnywhere)
  UResourceNetworkManager *ResourceNetworkManager;

  private:
  // Columns

  friend class AColumn;
  friend class USectorArea;

  struct ColumnDataKeyFuncs {
    typedef Vec3i KeyType;
    typedef typename TCallTraits<Vec3i>::ParamType KeyInitType;
    typedef typename TCallTraits<AColumn *>::ParamType ElementInitType;

    enum {
      bAllowDuplicateKeys = false
    };

    static FORCEINLINE KeyInitType GetSetKey(ElementInitType Element) { return Element->pos; }

    static FORCEINLINE bool Matches(KeyInitType A, KeyInitType B) { return A == B; }

    static FORCEINLINE uint32 GetKeyHash(KeyInitType Key) { return GetTypeHash(Key); }
  };

  struct NearestColumn {
    AColumn *data;
    double dist;
  };

  private:
  std::unique_ptr<TThreadWorker<SectorCompilerData>> mSectorCompilerWorker;
  std::unique_ptr<TThreadWorker<FColumnLoaderData>> mSectorSaverWorker;

  UPROPERTY(VisibleAnywhere)
  TMap<FVector3i, AColumn *> mColumns;

  UPROPERTY(VisibleAnywhere)
  int32 mCountRemove = 0;

  UPROPERTY(VisibleAnywhere)
  int32 mCountCreate = 0;

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

  bool IsColumnActive(const AColumn &column) const;
  void SpawnColumn(Vec3i spos, AColumn *tall, FColumnLoaderData &data);

  bool IsColumnActive1(const AColumn &column) const;
  bool IsColumnRemove(const AColumn &column) const;
  bool IsColumnLoaded(const AColumn &column) const;
  bool IsColumnUsed(const AColumn &column) const;

  void CacheColumn(AColumn &column);
  void DecacheColumn(AColumn &column);

  void CacheSector(const Vec3i &pos, USectorProxy *sector);

  void GetNearestColumns(TMap<FVector3i, AColumn *> &data, TArray<NearestColumn> &out);

  public:
  UBlockLogic *GetBlockLogic(Vec3i bpos);
  void ReplaceSectorProxy(Vec3i spos, TScriptInterface<USectorProxy> old_proxy, TScriptInterface<USectorProxy> proxy,
                          AColumn *column);

  void KillNetworkDeffered(UBlockNetwork *mNetwork);

  private:
  UPROPERTY(VisibleAnywhere)
  FLazyGameSession GameSessionCache;
};
