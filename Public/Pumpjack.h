// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once
#include "DrillingMachineBase.h"
#include "Pumpjack.generated.h"

class URegionLayer;
/**
 * Specialized drilling machine that extracts oil from oil deposits.
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
      .addProperty("layer", &Self::Layer) //@field RegionLayer
      .endClass();
  }

  UPumpjack();

  public:
  std::tuple<URegionLayer *, FVector2i> GetLayer() const;

  int32 GetTimePerRecipe() const;

  virtual float GetMiningProgress() const override;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drilling|Resource")
  URegionLayer *Layer = nullptr;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drilling|Resource")
  FVector2i Subregion;

  virtual TSubclassOf<UBlockWidget> GetWidgetClass() const override;

  UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
  int32 LastSpeed = 100; // speed is deprecated in logic, kept for UI compatibility

  UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
  int32 CurrentRecipeTime = 0;

  UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Drilling|Resource")
  bool IsRegisteredWithLayer = false;

  protected:
  virtual void Drill() override;

  virtual void BlockEndPlay() override;
};