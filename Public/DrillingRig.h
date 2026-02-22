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

  UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Drilling|Resource")
  int32 LastSpeed = 100;

  UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Drilling|Resource")
  int32 CurrentRecipeTime = 0;

  UFUNCTION(BlueprintCallable, Category = "Drilling|Stats")
  int32 GetTimePerRecipe() const;

  virtual TSubclassOf<UBlockWidget> GetWidgetClass() const override;
  virtual UStaticItem *GetSelectedOption() const override;

  protected:
  virtual float GetMiningProgress() const override;
  virtual void Drill() override;
  virtual void BlockEndPlay() override;
};
