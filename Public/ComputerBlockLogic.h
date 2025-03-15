// Copyright (c) 2017 - 2024, Samsonov Andrey. All Rights Reserved.
#pragma once
#include "Public/TieredBlockLogic.h"
#include "CoreMinimal.h"

#include "ComputerBlockLogic.generated.h"

class UResourceAccessor;
class UResourceInventory;
class USingleSlotInventory;
class UInventoryContainer;
class UCoreAccessor;
class UElectricInputAccessor;

UCLASS()
class EVOSPACE_API UComputerBlockLogic : public UBlockLogic {
  GENERATED_BODY()
  using Self = UComputerBlockLogic;
  EVO_CODEGEN_INSTANCE(ComputerBlockLogic)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UBlockLogic>("ComputerBlockLogic") //@class ComputerBlockLogic : BlockLogic
      .addProperty("energy_inventory", &Self::energy_inventory) //@field ResourceInventory
      .addProperty("energy_input", &Self::energy_input) //@field ResourceAccessor
      .endClass();
  }
  public:
  UComputerBlockLogic();

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  UResourceInventory *energy_inventory;

  FByteProperty *mStatusProperty = nullptr;
  FByteProperty *mLastTierProperty = nullptr;

  virtual TSubclassOf<UBlockWidget> GetWidgetClass() const override;

  virtual bool IsBlockTicks() const override;

  int8 TicksToGet = 20;
  int8 mStatus = 0;
  int8 mLastTier = 0;

  virtual void SetStatus(int8 w);
  virtual void SetLastTier(int8 w);

  virtual void SetActor(ABlockActor *actor) override;

  virtual void BlockBeginPlay() override;

  protected:
  virtual void Tick() override;

  UPROPERTY()
  UResourceAccessor *energy_input;

  UPROPERTY(BlueprintReadWrite)
  USingleSlotInventory *chip_inventory;

  UPROPERTY()
  const UStaticItem *mCompItem = nullptr;

  UPROPERTY(BlueprintReadWrite)
  int32 mBoostLeft = 0;

  UPROPERTY(BlueprintReadWrite)
  int32 mBoostForce = 1;

  UPROPERTY(BlueprintReadWrite)
  int32 WorkingCD = 20;

  UPROPERTY()
  AMainPlayerController *mpc;
};