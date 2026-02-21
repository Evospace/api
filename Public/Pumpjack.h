// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once
#include "DrillingMachineBase.h"
#include "Pumpjack.generated.h"

class URegionLayer;
class UStaticItem;
/**
 * Drilling machine that extracts oil. Does not use regions; outputs constant rate.
 * Oil region layer is being removed; future ore-based oil will use SourceData.
 */
UCLASS()
class UPumpjack : public UDrillingMachineBase {
  GENERATED_BODY()

  public:
  using Self = UPumpjack;
  EVO_CODEGEN_INSTANCE(Pumpjack)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UDrillingMachineBase>("Pumpjack") //@class Pumpjack : DrillingMachineBase
      .addProperty("layer", &Self::Layer) //@field RegionLayer, deprecated, always nullptr
      .endClass();
  }

  UPumpjack();

  public:
  int32 GetTimePerRecipe() const;

  virtual float GetMiningProgress() const override;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drilling|Resource")
  URegionLayer *Layer = nullptr; // Deprecated, always nullptr. Kept for BP compat.

  virtual TSubclassOf<UBlockWidget> GetWidgetClass() const override;

  UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
  int32 LastSpeed = 100; // speed is deprecated in logic, kept for UI compatibility

  UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
  int32 CurrentRecipeTime = 0;

  protected:
  virtual void BlockBeginPlay() override;
  virtual void Drill() override;

  virtual void BlockEndPlay() override;

  private:
  UPROPERTY()
  const UStaticItem *OutputItem = nullptr;
};