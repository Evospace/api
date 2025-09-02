// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "../CoordinateSystem.h"
#include "Evospace/Blocks/BlockWidget.h"
#include "CoreMinimal.h"
#include "StorageBlockLogic.h"

#include "Public/SwitchInterface.h"
#include "Evospace/Props/DimensionPropComponent.h"
#include "Public/EvoRingBuffer.h"

#include "ConductorBlockLogic.generated.h"

class USectorProxy;
class UResourceAccessor;
class UStaticProp;
class USwitchBlockLogic;
class UConductorBlockLogic;
class UBaseInventoryAccessor;
class UGraphStorage;
class USingleSlotInventory;
class UBlockLogic;
class UAccessor;
class AItemLogic;

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
  TArray<UConductorBlockLogic *> Storages;

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
class EVOSPACE_API UBlockNetwork : public UObject {
  GENERATED_BODY()

  public:
  UBlockNetwork();

  bool AddSwitch(USwitchBlockLogic *a);

  bool IsEmpty() const;

  UPROPERTY(VisibleAnywhere)
  TArray<USubnetwork *> mSubnetworks;

  static TArray<TArray<UResourceAccessor *>> CheckIntegrity(UConductorBlockLogic *block);

  void DeferredKill();

  bool IsKillDeffered() const { return mKillDeffered; }

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
  float GetTotalDrain() const;

  UFUNCTION(BlueprintCallable)
  float GetTotalProduction(int32 sub) const;

  UFUNCTION(BlueprintCallable)
  bool AddCharge(int64 addition);

  UFUNCTION()
  void EndTick();

  static std::tuple<int64, int64> CalculateThermalLoss(int64 power_W, int64 voltage_V, int64 resistance_mOhm, int64 currentLimit_mA);

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UGraphStorage *mConsumptionStorage;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UGraphStorage *mProductionStorage;

  int64 mPerWireCon = 0;

  void ClearCollectedDirty();

  bool mCollectedDirty = true;

  private:
  const UStaticItem *mResource = nullptr;

  bool mKillDeffered = false;

  int32 mLifetime = 0;

  bool mUpdated = false;

  public:
  void ResizePerSubnetwork(int32 size);

  private:
  TArray<int64> Request;
  TArray<int64> TotalProduction;
  TArray<TArray<UResourceAccessor *>> AllCollectedOutputs;
  TArray<TArray<UResourceAccessor *>> AllCollectedInputs;
  TArray<TArray<UConductorBlockLogic *>> AllCollectedStorages;
  TArray<TArray<int32>> AllCollectedSubnetworks;

  public:
  UFUNCTION(BlueprintCosmetic, BlueprintCallable)
  FNetworkWidgetData GetWidgetData(int32 sub);

  private:
  // Widget data cache, updates only by widget data request
  mutable EvoRingBuffer<float> medianCharge = EvoRingBuffer<float>(20, 0);
  mutable EvoRingBuffer<float> medianRequest = EvoRingBuffer<float>(20, 0);
  mutable EvoRingBuffer<float> medianProduction = EvoRingBuffer<float>(20, 0);
};

/// Conductors

UCLASS(BlueprintType)
class EVOSPACE_API UConductorBlockLogic : public UStorageBlockLogic {
  using Self = UConductorBlockLogic;
  EVO_CODEGEN_INSTANCE(ConductorBlockLogic)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UStorageBlockLogic>("ConductorBlockLogic") //@class ConductorBlockLogic : StorageBlockLogic
      .addProperty("side_cover", &Self::mSideCover) //@field StaticCover
      .addProperty("center_cover", &Self::mCenterCover) //@field StaticCover
      .addProperty("channel", QR_NAME_GET_SET(Channel)) //@field string
      .addProperty("conductor_channel", &Self::ConductorChannel) //@field integer
      //direct:
      //---Add side wire
      //---@param acc ResourceAccessor
      //function ConductorBlockLogic:add_wire(acc) end
      .addFunction("add_wire", [](UConductorBlockLogic *self, UResourceAccessor *acc) {
        self->mWireAccessor.Add(acc);
      }) //@field integer
      .endClass();
  }
  GENERATED_BODY()

  public:
  virtual void NeighborBlockAdded(UBlockLogic *block, const Vec3i &pos) override;
  virtual void NeighborBlockRemoved(UBlockLogic *block, const Vec3i &pos) override;

  virtual bool IsResourceStorage() const { return false; }
  virtual bool IsBatteryContainer() const { return false; }

  virtual int64 GetCapacity() const override;

  UFUNCTION(BlueprintCallable, BlueprintPure)
  virtual int64 GetCharge() const;

  virtual Vec3i GetRotationLocks() const override;

  UFUNCTION(BlueprintCallable)
  virtual int64 GetPerWire() const { return 0; }

  virtual TSubclassOf<UBlockWidget> GetWidgetClass() const override;

  virtual TArray<ConnectionInfo> GetConnectionInfo() const;

  virtual int32 GetChannel() const;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int32 ConductorChannel;

  virtual EBlockWidgetType GetWidgetType() const override;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  int mSides = 0;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  FName Channel = "Any";

  UPROPERTY(VisibleAnywhere)
  UStaticIndexedHierarchicalInstancedStaticMeshComponent *mComponent;

  int32 mIndex;

  bool CheckPlaceble(ADimension *dim, FVector3i pos) override;

  protected:
  virtual void BlockBeginPlay() override;

  UPROPERTY(VisibleAnywhere)
  UStaticCover *mCover;

  UPROPERTY(VisibleAnywhere)
  UStaticCover *mSideCover;

  UPROPERTY(VisibleAnywhere)
  UStaticCover *mCenterCover;

  void RebuildFrom(int64 rem_capacity = 0);

  virtual void BlockEndPlay() override;

  virtual void Tick() override;

  protected:
  TArray<UResourceAccessor *> mWireAccessor;

  virtual void UpdateSides(UAccessor *except = nullptr);

  //==================================================================
  public:
  friend UBlockNetwork;
  friend USwitchBlockLogic;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;

  virtual bool Connect(UConductorBlockLogic *block);

  virtual bool Disconnect(UConductorBlockLogic *block);

  virtual bool IsDataNetwork() const;

  virtual void OnAction(const FHitResult &hit, const Vec3i &side, AItemLogic *item) override;

  UFUNCTION(BlueprintCallable)
  UBlockNetwork *GetNetwork() const;

  virtual void SetRenderable(AColumn *sector) override;

  virtual void RemoveActorOrRenderable() override;

  TArray<RCoverWrapper> SideCovers;

  RCoverWrapper CenterCover;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UBlockNetwork *mNetwork = nullptr;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  int32 Subnetwork = 0;
};

UCLASS()
class EVOSPACE_API UHeatConductorBlockLogic : public UConductorBlockLogic {
  GENERATED_BODY()
  public:
  UHeatConductorBlockLogic();

  virtual int64 GetPerWire() const override;

  virtual int32 GetChannel() const override { return 3000; }

  virtual bool IsResourceStorage() const override { return true; }
  virtual bool IsBatteryContainer() const override { return false; }

  virtual int64 GetCapacity() const override;

  UPROPERTY(VisibleAnywhere)
  class UResourceInventory *mStorage;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  float mBaseCapacity = 1.f;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  float mBonusCapacity = 1.f;

  bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  virtual void UpdateSides(UAccessor *except) override;

  int64 GetCharge() const override;
};

UCLASS()
class EVOSPACE_API UDataConductorBlockLogic : public UConductorBlockLogic {
  GENERATED_BODY()
  public:
  UDataConductorBlockLogic();

  virtual TSubclassOf<UBlockWidget> GetWidgetClass() const override;

  virtual bool IsDataNetwork() const override { return true; }

  virtual int32 GetChannel() const override { return 6000; }

  void UpdateSides(UAccessor *except) override;
};

UCLASS()
class EVOSPACE_API UPlasmaConductorBlockLogic : public UHeatConductorBlockLogic {
  GENERATED_BODY()
  public:
  UPlasmaConductorBlockLogic();

  virtual int32 GetChannel() const override { return 5000; }
};

UCLASS()
class EVOSPACE_API UKineticConductorBlockLogic : public UConductorBlockLogic {
  GENERATED_BODY()

  public:
  UKineticConductorBlockLogic();
  virtual int64 GetPerWire() const override;

  virtual int32 GetChannel() const override { return 4000; }

  virtual int64 GetCapacity() const override;

  virtual bool IsResourceStorage() const override { return true; }
  virtual bool IsBatteryContainer() const override { return false; }

  virtual void UpdateSides(UAccessor *except) override;

  virtual bool IsBlockTicks() const override;

  UPROPERTY()
  UResourceInventory *mStorage;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  float mBaseCapacity = 1.f;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  float mBonusCapacity = 1.f;

  bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  int64 GetCharge() const override;
};

/// Switches

UCLASS()
class EVOSPACE_API USwitchBlockLogic : public UConductorBlockLogic, public ISwitchInterface {
  GENERATED_BODY()

  public:
  virtual TSubclassOf<UBlockWidget> GetWidgetClass() const override;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;

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

  virtual void UpdateSides(UAccessor *except = nullptr) override;

  virtual bool IsBlockTicks() const override;

  void SetSwitch_Implementation(bool val) override;

  bool GetSwitch_Implementation() const override;
  virtual TArray<ConnectionInfo> GetConnectionInfo() const override;
};

UCLASS()
class EVOSPACE_API UElectricitySwitchBlockLogic : public USwitchBlockLogic {
  GENERATED_BODY()
  public:
  virtual void SpawnBlockPostprocess() override;
  UElectricitySwitchBlockLogic();
  virtual int32 GetChannel() const override;
};

UCLASS()
class EVOSPACE_API UFluidSwitchBlockLogic : public USwitchBlockLogic {
  GENERATED_BODY()
  public:
  virtual void SpawnBlockPostprocess() override;
  UFluidSwitchBlockLogic();
  virtual int32 GetChannel() const override;
};

UCLASS()
class EVOSPACE_API UHeatSwitchBlockLogic : public USwitchBlockLogic {
  GENERATED_BODY()
  public:
  virtual void SpawnBlockPostprocess() override;
  UHeatSwitchBlockLogic();
  virtual int32 GetChannel() const override;
};

UCLASS()
class EVOSPACE_API UKineticSwitchBlockLogic : public USwitchBlockLogic {
  GENERATED_BODY()
  public:
  virtual void SpawnBlockPostprocess() override;
  UKineticSwitchBlockLogic();
  virtual int32 GetChannel() const override;
};

UCLASS()
class EVOSPACE_API UDataSwitchBlockLogic : public USwitchBlockLogic {
  GENERATED_BODY()
  public:
  virtual void SpawnBlockPostprocess() override;
  UDataSwitchBlockLogic();
  virtual int32 GetChannel() const override;
};