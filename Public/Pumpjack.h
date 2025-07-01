// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once
#include "DrillingMachineBase.h"
#include "Pumpjack.generated.h"

class URegionLayer;
/**
* Specialized drilling machine that extracts oil from oil deposits.
 */
UCLASS()
class EVOSPACE_API UPumpjack : public UDrillingMachineBase {
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

  public:
  std::tuple<URegionLayer *, FVector2i> GetLayer() const;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drilling|Resource")
  URegionLayer *Layer = nullptr;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drilling|Resource")
  FVector2i Subregion;

  virtual TSubclassOf<UBlockWidget> GetWidgetClass() const override;

  protected:
  virtual void Drill() override;
};