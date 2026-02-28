// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once
#include "DrillingMachineBase.h"
#include "Pumpjack.generated.h"

class URegionLayer;
class USourceData;
class UStaticItem;
/**
 * Drilling machine that extracts oil from deposits.
 * Non-legacy: uses FindSource() to find oil deposits (OilCluster) created by OreGenerator.
 * Legacy (Pumpjack_leg): always extracts oil without source checks.
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
      .addProperty("source", &Self::Source) //@field SourceData
      .endClass();
  }

  UPumpjack();

  public:
  virtual float GetMiningProgress() const override;
  virtual USourceData *GetSource() const override { return Source; }

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drilling|Resource")
  USourceData *Source = nullptr;

  virtual TSubclassOf<UBlockWidget> GetWidgetClass() const override;
  virtual UStaticItem *GetSelectedOption() const override;

  protected:
  virtual void BlockBeginPlay() override;
  virtual void Drill() override;
  virtual void BlockEndPlay() override;

  private:
  bool IsLegacyPumpjack() const;

  UPROPERTY()
  const UStaticItem *OutputItem = nullptr;
};