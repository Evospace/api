// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "EvoRingBuffer.h"
#include "Evospace/ThreadWorker.h"
#include "Evospace/Vector.h"
#include "Evospace/Props/DimensionPropComponent.h"
#include "Evospace/Props/StaticIndexedHierarchicalInstancedStaticMeshComponent.h"
#include "Evospace/World/Column.h"
#include "Evospace/World/SectorCompiler.h"
#include "Public/StaticBlock.h"

#include <Containers/CircularBuffer.h>
#include <Engine/EngineTypes.h>
#include <Math/Vector.h>

#include <memory>

#include "Dimension.generated.h"

class UStaticCover;
class UGraphStorage;
class UGameSessionData;
class UStaticIndexedItemInstancedStaticMeshComponent;
class UBatchBlockLogicManager;
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
class UWorldGenerator;
class UStaticProp;
class UDimensionPropComponent;
class UStaticIndexedHierarchicalInstancedStaticMeshComponent;
class USectorCompiler;
class APlayerController;
class UDimensionLoadWidget;
class URegionMap;

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
      .addFunction("spawn_block", &ADimension::LuaSpawnBlockLogic)
      .addFunction("spawn_block_identity", &ADimension::SpawnLogicFullIdentity)
      .addFunction("set_cell", &ADimension::LuaSetBlockCell)
      .addProperty("settings", &ADimension::DimSettings) //@field GameSessionData
      .endClass();
  }

  ADimension();

  UFUNCTION(BlueprintCallable)
  void BeginTeleport(const FVector &pos, bool find_best_pos = true);

  UFUNCTION(BlueprintCallable)
  float GetTps() const;

  UFUNCTION(BlueprintCallable)
  float GetFps() const;

  UFUNCTION(BlueprintCallable)
  float GetTickDuration() const;

  public:
  // Core Functions

  ISectorProxy *GetSector(const Vec3i &spos);
  const ISectorProxy *GetSector(const Vec3i &spos) const;

  ISectorProxy *FindBlockCell(const Vec3i &pos, IndexType &index);
  const ISectorProxy *FindBlockCell(const Vec3i &pos, IndexType &index) const;

  UFUNCTION(BlueprintCallable)
  void SaveDimentionFolder(bool backup = false);

  USectorSaver *GetSectorSaver();

  void LoadDimentionFolder();
  void LuaSetBlockCell(const Vec3i &bpos, const UStaticBlock *cl);

  UBlockLogic *LuaSpawnBlockLogicDefault(const Vec3i &bpos, const UStaticBlock *cl);
  UBlockLogic *LuaSpawnBlockLogic(const Vec3i &bpos, const FQuat &block_q, const UStaticBlock *cl);

  FItemInstancingHandle AddItemInstance(const UStaticItem *prop, const FTransform &t) const;

  void RemoveInstance(const UStaticItem *prop, int32 index);

  UBlockLogic *SetBlockLogic(Vec3i pos, UBlockLogic *logic);
  UBlockLogic *DestroyLogicFull(const Vec3i &pos);
  UBlockLogic *SpawnLogicFullIdentity(const Vec3i &bpos, const UStaticBlock *staticObject);
  UBlockLogic *SpawnLogicFull(const Vec3i &bpos, const UStaticBlock *staticObject, const FQuat &rot);

  UBlockLogic *_SpawnLogicFull(const Vec3i &bpos, UBlockLogic *parent, const UStaticBlock *staticObject, const FQuat &rot);

  virtual void BeginPlay() override;

  void LoadDimJson();

  virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

  virtual void Tick(float DeltaTime) override;
  void AddBlockBuildCover(const TTuple<FVector3i, UBlockLogic *> &log) const;

  bool mBuildModeBoxes = false;

  bool mAccessorsPlanes = false;

  UFUNCTION(BlueprintCallable)
  void ActorBuildModeToggle(bool enabled);

  UFUNCTION(BlueprintCallable)
  void Pause();

  UFUNCTION(BlueprintCallable)
  void Unpause();

  UBlockNetwork *GetNewNetwork(int64 per_wire = 0);

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
  UGraphStorage *ResourceGraph;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
  UGameSessionData *DimSettings;

  UPROPERTY(VisibleAnywhere)
  UDimensionPropComponent *DimensionPropComponent;

  UPROPERTY()
  UStaticIndexedHierarchicalInstancedStaticMeshComponent *BuildingModCover;

  UPROPERTY()
  TMap<const UStaticCover *, UStaticIndexedHierarchicalInstancedStaticMeshComponent *> mAccessorCover;

  UPROPERTY(EditAnywhere)
  AWorldFeaturesManager *FeatureManager;

  UPROPERTY(EditAnywhere)
  UBatchBlockLogicManager *BatchManager;

  protected:
  // Core Tick

  float HalfSecond = 0.f;
  float DeltaRemain = 0.f;
  int32 Ticks = 0;
  int32 HalfTps = 0;
  int32 FrameCount = 0;
  EvoRingBuffer<float> TpsBuffer = EvoRingBuffer<float>(16, 0);
  EvoRingBuffer<float> CyclesBuffer = EvoRingBuffer<float>(16, 0);
  EvoRingBuffer<float> FpsBuffer = EvoRingBuffer<float>(4, 0);
  bool Paused = false;
  int32 MaxTps = 20;

  UPROPERTY(EditAnywhere)
  bool mTicking = true;

  UPROPERTY(EditAnywhere)
  bool mNetworkTicking = true;

  FTSTicker::FDelegateHandle TickDelegateHandle;

  private:
  UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
  bool bSectorGenegateOnly = true;

  UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
  TSubclassOf<ASector> mSectorClass;

  void TeleportProcess();
  void LoadingProcess();

  bool mPlayerTeleported = false;
  bool mPlayerTeleportedBest = false;
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
  TMap<FVector3i, UBlockLogic *> mDimensionLogicsTick;

  UPROPERTY()
  TSet<UBlockNetwork *> mBlockNetworks;

  UPROPERTY()
  TSet<UBlockNetwork *> mNetworksToKill;

  private:
  // Columns

  friend class UColumn;

  struct ColumnDataKeyFuncs {
    typedef Vec3i KeyType;
    typedef typename TCallTraits<Vec3i>::ParamType KeyInitType;
    typedef typename TCallTraits<UColumn *>::ParamType ElementInitType;

    enum {
      bAllowDuplicateKeys = false
    };

    static FORCEINLINE KeyInitType GetSetKey(ElementInitType Element) {
      return Element->pos;
    }

    static FORCEINLINE bool Matches(KeyInitType A, KeyInitType B) {
      return A == B;
    }

    static FORCEINLINE uint32 GetKeyHash(KeyInitType Key) {
      return GetTypeHash(Key);
    }
  };

  struct NearestColumn {
    UColumn *data;
    double dist;
  };

  private:
  std::unique_ptr<TThreadWorker<SectorCompilerData>> mSectorCompilerWorker;
  std::unique_ptr<TThreadWorker<FColumnSectorsData>> mSectorSaverWorker;

  UPROPERTY(VisibleAnywhere)
  TMap<FVector3i, UColumn *> mColumns;

  UPROPERTY(VisibleAnywhere)
  int32 mCountRemove = 0;

  UPROPERTY(VisibleAnywhere)
  int32 mCountCreate = 0;

  public:
  void LoadColumn(const Vec3i &pos);
  void UnloadColumn(const Vec3i &pos);

  bool IsCanCreateColumn() const;
  bool IsCanRemoveColumn() const;
  bool IsCanSaveColumn(const UColumn &tall) const;
  bool IsColumnChanged(const UColumn &tall) const;

  private:
  void TickProcess(float DeltaTime);
  bool TickBlocks(float DeltaTime);

  bool IsColumnActive(const UColumn &column) const;
  void SpawnColumn(Vec3i spos, UColumn *tall, FColumnSectorsData &data);

  bool IsColumnActive1(const UColumn &column) const;
  bool IsColumnRemove(const UColumn &column) const;
  bool IsColumnLoaded(const UColumn &column) const;
  bool IsColumnUsed(const UColumn &column) const;

  void CacheColumn(UColumn &column);
  void DecacheColumn(UColumn &column);

  void CacheSector(const Vec3i &pos, ISectorProxy *sector);

  void GetNearestColumns(TMap<FVector3i, UColumn *> &data, TArray<NearestColumn> &out);

  public:
  UBlockLogic *GetBlockLogic(Vec3i bpos);

  void KillNetworkDeffered(UBlockNetwork *mNetwork);
};
