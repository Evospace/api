// Copyright (c) 2017 - 2024, Samsonov Andrey. All Rights Reserved.
#pragma once
#include "../CoordinateSystem.h"
#include "Evospace/Blocks/BlockWidget.h"
#include "CoreMinimal.h"
#include "StorageBlockLogic.h"
#include "Evospace/Blocks/ItemHopperInterface.h"
#include "Evospace/Blocks/SidesArray.h"
#include "Evospace/Props/DimensionPropComponent.h"
#include "Public/EvoRingBuffer.h"

#include "ConductorBlockLogic.generated.h"

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
  TArray<UResourceAccessor *> mPossibleAccessors;

  UPROPERTY(VisibleAnywhere)
  TArray<UResourceAccessor *> mInputs;

  UPROPERTY(VisibleAnywhere)
  TArray<UResourceAccessor *> mOutputs;

  UPROPERTY(VisibleAnywhere)
  TArray<UConductorBlockLogic *> mStorages;

  UPROPERTY(VisibleAnywhere)
  int64 mCharge = 0;

  UPROPERTY(VisibleAnywhere)
  int64 mCapacity = 0;
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

  UFUNCTION(BlueprintCosmetic, BlueprintCallable)
  FNetworkWidgetData GetWidgetData();

  UPROPERTY(VisibleAnywhere)
  TArray<UConductorBlockLogic *> mWires;

  protected:
  UPROPERTY(VisibleAnywhere)
  TArray<USwitchBlockLogic *> mSwitches;

  UPROPERTY()
  UStaticItem *mBatteryItem;

  UPROPERTY()
  UStaticItem *mDrainItem;

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
  float GetCharge(int32 sub = 0) const;

  UFUNCTION(BlueprintCallable)
  float GetCapacity(int32 sub = 0) const;

  UFUNCTION(BlueprintCallable)
  float GetRequest() const;

  int64 GetCharge64(int32 sub = 0) const;
  int64 GetCapacity64(int32 sub = 0) const;

  UFUNCTION(BlueprintCallable)
  float GetTotalDrain() const;

  UFUNCTION(BlueprintCallable)
  float GetTotalProduction() const;

  UFUNCTION(BlueprintCallable)
  bool AddCharge(int64 addition);

  UFUNCTION()
  void EndTick();

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

  int64 mRequest = 0;
  int64 mTotalProduction = 0;

  TArray<TArray<UResourceAccessor *>> AllCollectedOutputs;
  TArray<TArray<UResourceAccessor *>> AllCollectedInputs;
  TArray<TArray<UConductorBlockLogic *>> AllCollectedStorages;
  TArray<TArray<int32>> AllCollectedSubnetworks;

  EvoRingBuffer<float> medianCharge = EvoRingBuffer<float>(20, 0);
  EvoRingBuffer<float> medianRequest = EvoRingBuffer<float>(20, 0);
  EvoRingBuffer<float> medianProduction = EvoRingBuffer<float>(20, 0);
};

/// Conductors

UCLASS(Abstract)
class EVOSPACE_API UConductorBlockLogic : public UStorageBlockLogic {
  using Self = UConductorBlockLogic;
  EVO_CODEGEN_INSTANCE(ConductorBlockLogic)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UStorageBlockLogic>("ConductorBlockLogic") //class: ConductorBlockLogic, parent: StorageBlockLogic
      .addProperty("side_cover", &Self::mSideCover) //field: StaticCover
      .addProperty("center_cover", &Self::mCenterCover) //field: StaticCover
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

  bool SetChannel(int32 channel);
  virtual int32 GetChannel() const;

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

  virtual void SetRenderable() override;

  virtual void RemoveActorOrRenderable() override;

  TArray<int32> mSideIndexes;
  int32 mCenterInxed = INDEX_NONE;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UBlockNetwork *mNetwork = nullptr;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  int32 mSubnetwork = INDEX_NONE;

  int32 mChannel = 0;
};

UCLASS()
class EVOSPACE_API UFluidConductorBlockLogic : public UConductorBlockLogic {
  GENERATED_BODY()
  public:
  UFluidConductorBlockLogic();

  protected:
  virtual void BlockBeginPlay() override;
};

UCLASS()
class EVOSPACE_API UElectricityConductorBlockLogic : public UConductorBlockLogic {
  GENERATED_BODY()
  public:
  UElectricityConductorBlockLogic();

  virtual int32 GetChannel() const override;
};

UCLASS()
class EVOSPACE_API UDataConductorBlockLogic : public UConductorBlockLogic {
  GENERATED_BODY()
  public:
  UDataConductorBlockLogic();

  virtual TSubclassOf<UBlockWidget> GetWidgetClass() const override;

  virtual int32 GetChannel() const override { return 2000; }

  bool IsDataNetwork() const override;
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

  protected:
  virtual void BlockBeginPlay() override;
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

  bool IsSwithedOn() const;

  UPROPERTY(VisibleAnywhere)
  bool mSwitchedOn = false;

  UPROPERTY(VisibleAnywhere)
  int32 mSubnetwork1 = INDEX_NONE;

  UPROPERTY(VisibleAnywhere)
  int32 mSubnetwork2 = INDEX_NONE;

  virtual void SetActor(ABlockActor *actor) override;

  virtual void UpdateSides(UAccessor *except = nullptr) override;

  virtual bool IsBlockTicks() const override;

  void SetSwitch(bool val) override;

  bool GetSwitch() const override;
  virtual TArray<ConnectionInfo> GetConnectionInfo() const override;
};

UCLASS()
class EVOSPACE_API UElectricitySwitchBlockLogic : public USwitchBlockLogic {
  GENERATED_BODY()
  public:
  UElectricitySwitchBlockLogic();
  virtual int32 GetChannel() const override;
};

UCLASS()
class EVOSPACE_API UFluidSwitchBlockLogic : public USwitchBlockLogic {
  GENERATED_BODY()
  public:
  UFluidSwitchBlockLogic();
  virtual int32 GetChannel() const override;
};

UCLASS()
class EVOSPACE_API UHeatSwitchBlockLogic : public USwitchBlockLogic {
  GENERATED_BODY()
  public:
  UHeatSwitchBlockLogic();
  virtual int32 GetChannel() const override;
};

UCLASS()
class EVOSPACE_API UKineticSwitchBlockLogic : public USwitchBlockLogic {
  GENERATED_BODY()
  public:
  UKineticSwitchBlockLogic();
  virtual int32 GetChannel() const override;
};

UCLASS()
class EVOSPACE_API UDataSwitchBlockLogic : public USwitchBlockLogic {
  GENERATED_BODY()
  public:
  UDataSwitchBlockLogic();
  virtual int32 GetChannel() const override;
};