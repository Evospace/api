// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "../CoordinateSystem.h"
#include "Public/BlockWidget.h"
#include "CoreMinimal.h"
#include "Public/SwitchInterface.h"
#include "Evospace/Props/DimensionPropComponent.h"
#include "Public/EvoRingBuffer.h"
#include "Public/LazyGameSessionData.h"
#include "Public/LogicSettingsBlockLogic.h"

#include "CoverAttachInterface.h"

#include "ConductorBlockLogic.generated.h"

class USectorProxy;
class UResourceAccessor;
class UStaticProp;
class UStaticItem;
class USwitchBlockLogic;
class UConductorBlockLogic;
class UBaseInventoryAccessor;
class UNetworkResourceInventory;
class UGraphStorage;
class USingleSlotInventory;

struct FNetworkWidgetMedianBuffers {
  EvoRingBuffer<float> Charge = EvoRingBuffer<float>(20, 0.f);
  EvoRingBuffer<float> Request = EvoRingBuffer<float>(20, 0.f);
  EvoRingBuffer<float> Production = EvoRingBuffer<float>(20, 0.f);
};
class UBlockLogic;
class UAccessor;
class AItemLogic;
class UGameSessionData;
class UResourceInventory;
class UCoreAccessor;
class ULogicContext;

UCLASS(BlueprintType)
class USubnetwork : public UObject {
  GENERATED_BODY()
  public:
  UPROPERTY(VisibleAnywhere)
  int32 id;

  UPROPERTY(VisibleAnywhere)
  TArray<UResourceAccessor *> PossibleAccessors;

  UPROPERTY(VisibleAnywhere)
  TArray<UResourceAccessor *> Inputs;

  UPROPERTY(VisibleAnywhere)
  TArray<UResourceAccessor *> Outputs;

  UPROPERTY(VisibleAnywhere)
  int64 Charge = 0;

  UPROPERTY(VisibleAnywhere)
  int64 Capacity = 0;
};

class ConnectionInfo {
  public:
  TArray<UResourceAccessor *> thisSubnetworkSides;
};

USTRUCT(BlueprintType)
struct FNetworkWidgetData {
  GENERATED_BODY()

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  float Prognoses = 0;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  float meanProduction = 0;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  float meanConsumption = 0;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  float meanFactor = 0;
};

UCLASS(BlueprintType)
class UBlockNetwork : public UObject {
  GENERATED_BODY()

  public:
  void InitializeNetwork();

  bool AddSwitch(USwitchBlockLogic *a);

  bool IsEmpty() const;

  UPROPERTY(VisibleAnywhere)
  TArray<USubnetwork *> mSubnetworks;

  static TArray<TArray<UResourceAccessor *>> CheckIntegrity(UConductorBlockLogic *block);

  void DeferredKill();

  bool IsKillDeferred() const { return mKillDeferred; }

  void SetResource(const UStaticItem *param1);

  UPROPERTY(VisibleAnywhere)
  TArray<UConductorBlockLogic *> mWires;

  protected:
  UPROPERTY(VisibleAnywhere)
  TArray<USwitchBlockLogic *> mSwitches;

  UPROPERTY()
  UStaticItem *mBatteryItem;

  UPROPERTY()
  UStaticItem *mDrainItem;

  UPROPERTY()
  UStaticItem *mHeatLoss;

  UPROPERTY(BlueprintReadWrite)
  UMaterialInstanceDynamic *mSharedMaterial = nullptr;

  UPROPERTY(BlueprintReadWrite)
  bool mSharedMaterialUpdate = false;

  public:
  UPROPERTY(VisibleAnywhere)
  bool mIsDataNetwork = false;

  void Tick();

  UFUNCTION(BlueprintCallable)
  UStaticItem *GetResource() const;

  UFUNCTION(BlueprintCallable)
  void ResetResource();

  UFUNCTION(BlueprintCallable)
  float GetCharge(int32 sub) const;

  UFUNCTION(BlueprintCallable)
  float GetCapacity(int32 sub) const;

  UFUNCTION(BlueprintCallable)
  float GetRequest(int32 sub) const;

  int64 GetCharge64(int32 sub) const;
  int64 GetCapacity64(int32 sub) const;

  UFUNCTION(BlueprintCallable)
  int64 GetTotalDrain() const;

  UFUNCTION(BlueprintCallable)
  float GetTotalProduction(int32 sub) const;

  void CaptureStorageSnapshot();
  void RestoreStorageSnapshot();
  void ClearStorageSnapshot();

  UFUNCTION()
  void EndTick();

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UGraphStorage *mConsumptionStorage;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UGraphStorage *mProductionStorage;

  UPROPERTY(VisibleAnywhere)
  int64 CachedDrain = 0;

  void InvalidateCollected();

  bool mCollectedDirty = true;

  private:
  const UStaticItem *mResource = nullptr;

  bool mKillDeferred = false;

  int32 mLifetime = 0;

  bool mUpdated = false;

  public:
  void ResizePerSubnetwork(int32 size);

  private:
  TArray<int64> Request;
  TArray<int64> TotalProduction;
  TArray<TArray<UResourceAccessor *>> AllCollectedOutputs;
  TArray<TArray<UResourceAccessor *>> AllCollectedInputs;
  TArray<TArray<int32>> AllCollectedSubnetworks;

  UPROPERTY()
  ULogicContext *ReusableTempCtx = nullptr;

  UPROPERTY()
  ULogicContext *ReusableInCtx = nullptr;

  public:
  UFUNCTION(BlueprintCosmetic, BlueprintCallable)
  FNetworkWidgetData GetWidgetData(int32 sub);

  private:
  // Per-subnetwork widget medians; resized with ResizePerSubnetwork.
  mutable TArray<FNetworkWidgetMedianBuffers> mWidgetMedians;

  UPROPERTY(VisibleAnywhere)
  FLazyGameSession GameSessionCache;
};

/// Conductors

UCLASS(BlueprintType)
class UConductorBlockLogic : public ULogicSettingsBlockLogic, public ICoverAttachInterface {
  using Self = UConductorBlockLogic;
  EVO_CODEGEN_INSTANCE(ConductorBlockLogic)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UBlockLogic>("ConductorBlockLogic") //@class ConductorBlockLogic : BlockLogic
      .addProperty("side_cover", &Self::mSideCover) //@field StaticCover
      .addProperty("center_cover", &Self::mCenterCover) //@field StaticCover
      .addProperty("channel", QR_NAME_GET_SET(Channel)) //@field string
      .addProperty("conductor_channel", &Self::ConductorChannel) //@field integer
      .addProperty("capacity", &Self::Capacity) //@field integer
      .addProperty("drain", &Self::Drain) //@field integer
      // direct:
      //---Add side wire
      //---@param acc ResourceAccessor
      // function ConductorBlockLogic:add_wire(acc) end
      .addFunction("add_wire", [](UConductorBlockLogic *self, UResourceAccessor *acc) { self->mWireAccessor.Add(acc); }) //@field integer
      .endClass();
  }
  GENERATED_BODY()

  public:
  virtual void PopulateLogicOutput(class ULogicContext *ctx) const override;

  virtual void NeighborBlockAdded(UBlockLogic *block, const Vec3i &pos) override;
  virtual void NeighborBlockRemoved(UBlockLogic *block, const Vec3i &pos) override;

  virtual int64 GetCapacity() const;
  virtual int64 GetCharge() const;

  virtual Vec3i GetRotationLocks() const override;

  virtual TSubclassOf<UBlockWidget> GetWidgetClass() const override;

  virtual TArray<ConnectionInfo> GetConnectionInfo() const;

  virtual int32 GetChannel() const;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int32 ConductorChannel;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  int64 Capacity = 0;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  int64 Drain = 0;

  virtual EBlockWidgetType GetWidgetType() const override;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  int mSides = 0;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  FName Channel = "Error";

  UPROPERTY(VisibleAnywhere)
  UStaticIndexedHierarchicalInstancedStaticMeshComponent *mComponent;

  int32 mIndex;

  bool CheckPlaceable(ADimension *dim, FQrVector3i pos) override;

  protected:
  virtual void BlockBeginPlay() override;

  UPROPERTY(VisibleAnywhere)
  UStaticCover *mSideCover;

  UPROPERTY(VisibleAnywhere)
  UStaticCover *mCenterCover;

  void RebuildFrom();

  virtual void BlockEndPlay() override;

  virtual void Tick() override;

  protected:
  TArray<UResourceAccessor *> mWireAccessor;

  virtual void UpdateSides(UAccessor *except = nullptr);

  // A conductor snapshots its share of network charge only while it is in the world;
  // a removed conductor must not take charge with it into a rebuild.
  bool SupportsLocalNetworkStorage() const {
    return IsInWorld();
  }
  void ResetStoredNetworkCharge();
  void AccumulateStoredNetworkCharge(UStaticItem *resource, int64 amount);
  void ApplyStoredNetworkCharge();
  int64 GetStoredNetworkCharge() const { return StoredNetworkCharge; }
  const UStaticItem *GetStoredNetworkResource() const { return StoredNetworkResource; }

  //==================================================================
  public:
  friend UBlockNetwork;
  friend USwitchBlockLogic;

  /** Rebuild connected network when this conductor is not attached to one yet. */
  void RebuildNetworkIfDetached();

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) const override;

  virtual bool Connect(UConductorBlockLogic *block);

  virtual bool Disconnect(UConductorBlockLogic *block);

  virtual bool IsDataNetwork() const;

  UFUNCTION(BlueprintCallable)
  UBlockNetwork *GetNetwork() const;

  virtual void SetRenderable(AColumn *sector) override;

  // Virtual network inventory for diagnostics / monitor; not exposed as Core (no item I/O on conductors).
  virtual UCoreAccessor *MonitorInit() override;

  virtual void RemoveActorOrRenderable() override;

  TArray<RCoverWrapper> SideCovers;

  RCoverWrapper CenterCover;

  // User-placed center cover support
  UPROPERTY(VisibleAnywhere)
  UStaticCover *UserCenterCover = nullptr;

  RCoverWrapper UserCenterCoverInstance;

  // ICoverAttachInterface
  virtual bool AttachCover(UStaticCover *cover) override;
  virtual bool DetachCover() override;
  virtual UStaticCover *GetAttachedCover() const override;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UBlockNetwork *mNetwork = nullptr;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  int32 Subnetwork = 0;

  UPROPERTY()
  UStaticItem *StoredNetworkResource = nullptr;

  UPROPERTY()
  int64 StoredNetworkCharge = 0;

  private:
  UPROPERTY(VisibleAnywhere)
  FLazyGameSession GameSessionCache;
};

UCLASS()
class UDataConductorBlockLogic : public UConductorBlockLogic {
  GENERATED_BODY()
  public:
  virtual TSubclassOf<UBlockWidget> GetWidgetClass() const override;

  virtual bool IsDataNetwork() const override { return true; }

  virtual int32 GetChannel() const override { return 6000; }
};

UCLASS()
class UKineticConductorBlockLogic : public UConductorBlockLogic {
  GENERATED_BODY()

  public:
  virtual bool IsBlockTicks() const override;
};

/// Switches

UCLASS()
class USwitchBlockLogic : public UConductorBlockLogic, public ISwitchInterface {
  GENERATED_BODY()

  public:
  virtual TSubclassOf<UBlockWidget> GetWidgetClass() const override;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) const override;

  virtual void AppendSimStateHash(struct FSimHashWriter &W) const override;

  virtual void OnAction(const FHitResult &hit, const Vec3i &side, AItemLogic *item) override;
  virtual bool HasAction() const override { return true; }

  bool IsSwithedOn() const;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
  bool mSwitchOn = false;

  UPROPERTY(VisibleAnywhere)
  int32 mSubnetwork1 = INDEX_NONE;

  UPROPERTY(VisibleAnywhere)
  int32 mSubnetwork2 = INDEX_NONE;

  UPROPERTY()
  UResourceAccessor *swAcc1 = nullptr;

  UPROPERTY()
  UResourceAccessor *swAcc2 = nullptr;

  virtual bool IsBlockTicks() const override;

  void SetSwitch_Implementation(bool val) override;

  bool GetSwitch_Implementation() const override;
  virtual TArray<ConnectionInfo> GetConnectionInfo() const override;

  virtual void ApplyLogicInput(const class ULogicContext *ctx) override;
};

UCLASS()
class UElectricitySwitchBlockLogic : public USwitchBlockLogic {
  GENERATED_BODY()
  public:
  virtual void SpawnBlockPostprocess() override;
  UElectricitySwitchBlockLogic();
  virtual int32 GetChannel() const override;
};

UCLASS()
class UFluidSwitchBlockLogic : public USwitchBlockLogic {
  GENERATED_BODY()
  public:
  virtual void SpawnBlockPostprocess() override;
  UFluidSwitchBlockLogic();
  virtual int32 GetChannel() const override;
};

UCLASS()
class UHeatSwitchBlockLogic : public USwitchBlockLogic {
  GENERATED_BODY()
  public:
  virtual void SpawnBlockPostprocess() override;
  UHeatSwitchBlockLogic();
  virtual int32 GetChannel() const override;
};

UCLASS()
class UKineticSwitchBlockLogic : public USwitchBlockLogic {
  GENERATED_BODY()
  public:
  virtual void SpawnBlockPostprocess() override;
  UKineticSwitchBlockLogic();
  virtual int32 GetChannel() const override;
};

UCLASS()
class UDataSwitchBlockLogic : public USwitchBlockLogic {
  GENERATED_BODY()
  public:
  virtual void SpawnBlockPostprocess() override;
  UDataSwitchBlockLogic();
  virtual int32 GetChannel() const override;
};