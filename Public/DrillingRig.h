// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DrillingMachineBase.h"
#include "Public/TieredBlockLogic.h"
#include "UObject/Object.h"
#include "DrillingRig.generated.h"

class USourceData;
/**
 * Drilling machine that extracts ore resources from the ground.
 */
UCLASS()
class UDrillingRig : public UDrillingMachineBase {
  GENERATED_BODY()
  using Self = UDrillingRig;
  EVO_CODEGEN_INSTANCE(DrillingRig)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UDrillingMachineBase>("DrillingRig") //@class DrillingRig : DrillingMachineBase
      .addProperty("source", &Self::Source) //@field SourceData
      .endClass();
  }

  UDrillingRig();

  public:
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drilling|Resource")
  USourceData *Source = nullptr;

  virtual TSubclassOf<UBlockWidget> GetWidgetClass() const override;

  protected:
  virtual void Drill() override;
};
