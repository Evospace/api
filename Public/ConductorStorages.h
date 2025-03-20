// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "ConductorBlockLogic.h"
#include "Evospace/Blocks/CommonBlockTypes/BatteryInterface.h"

#include "ConductorStorages.generated.h"

class UInventory;
class USingleSlotInventory;
class UAccessor;

UCLASS()
class EVOSPACE_API UBatteryBoxBlockLogic : public UElectricityConductorBlockLogic {
  GENERATED_BODY()

  public:
  UBatteryBoxBlockLogic();
  UBatteryBoxBlockLogic(float base, float bonus);

  virtual bool IsResourceStorage() const override { return true; }
  virtual bool IsBatteryContainer() const override { return false; }

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  virtual int64 GetCapacity() const override;

  int64 GetCharge() const override;

  virtual TSubclassOf<UBlockWidget> GetWidgetClass() const override;
  virtual EBlockWidgetType GetWidgetType() const override;

  protected:
  UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  USingleSlotInventory *mInventory;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  float mBaseCapacity = 1000000.f;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  float mBonusCapacity = 1000000.f;

  virtual void UpdateSides(UAccessor *except = nullptr) override;
  virtual int32 GetChannel() const override;
};

UCLASS()
class EVOSPACE_API UBatteryContainerBlockLogic : public UElectricityConductorBlockLogic {
  GENERATED_BODY()
  public:
  UBatteryContainerBlockLogic();

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UInventory *mInventory;

  virtual bool IsResourceStorage() const override { return true; }
  virtual bool IsBatteryContainer() const override { return true; }

  virtual void UpdateSides(UAccessor *except = nullptr) override;
  virtual int32 GetChannel() const override;

  virtual int64 GetCapacity() const override;
  virtual int64 GetCharge() const override;

  virtual void BlockBeginPlay() override;

  virtual TSubclassOf<UBlockWidget> GetWidgetClass() const override;
  virtual EBlockWidgetType GetWidgetType() const override;
};

UCLASS()
class EVOSPACE_API UFluidContainerBlockLogic : public UFluidConductorBlockLogic {
  GENERATED_BODY()

  public:
  UFluidContainerBlockLogic();

  virtual bool IsResourceStorage() const override { return true; }
  virtual bool IsBatteryContainer() const override { return false; }

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  virtual int64 GetCapacity() const override;
  virtual int64 GetCharge() const override;

  virtual void UpdateSides(UAccessor *except = nullptr) override;

  virtual TSubclassOf<UBlockWidget> GetWidgetClass() const override;
  virtual EBlockWidgetType GetWidgetType() const override;

  virtual int32 GetChannel() const override;

  protected:
  UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  UResourceInventory *mInventory;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  float mBaseCapacity = 30.f;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  float mBonusCapacity = 30.f;
};