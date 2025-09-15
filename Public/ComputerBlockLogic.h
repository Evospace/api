// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
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
class UComputerBlockLogic : public UBlockLogic {
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
  virtual UCoreAccessor *CoreInit() override;
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  UResourceInventory *energy_inventory;

  virtual TSubclassOf<UBlockWidget> GetWidgetClass() const override;

  virtual bool IsBlockTicks() const override;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  uint8 TicksToGet = 20;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  uint8 Status = 0;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  uint8 LastTier = 0;

  virtual void BlockBeginPlay() override;

  protected:
  virtual void Tick() override;

  UPROPERTY()
  UResourceAccessor *energy_input;

  UPROPERTY(BlueprintReadWrite)
  USingleSlotInventory *chip_inventory;

  UPROPERTY()
  const UStaticItem *CompItem = nullptr;

  UPROPERTY(BlueprintReadWrite)
  int32 BoostLeft = 0;

  UPROPERTY(BlueprintReadWrite)
  int32 BoostForce = 1;

  UPROPERTY(BlueprintReadWrite)
  int32 WorkingCD = 20;

  UPROPERTY()
  AMainPlayerController *mpc;
};