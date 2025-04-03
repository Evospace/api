// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "Public/SelectCrafter.h"
#include "CoreMinimal.h"
#include "Evospace/Blocks/ItemHopperInterface.h"

#include "AutoCrafter.generated.h"

class UAutosizeInventory;
class UInventory;

UCLASS(BlueprintType)
class EVOSPACE_API UAutoCrafter : public USelectCrafter {
  using Self = UAutoCrafter;
  EVO_CODEGEN_INSTANCE(AutoCrafter)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, USelectCrafter>("AutoCrafter") //@class AutoCrafter : SelectCrafter
      .endClass();
  }
  GENERATED_BODY()

  protected:
  UAutoCrafter();

  virtual void Tick() override;

  UPROPERTY()
  class UNonSerializedAutosizeInventory *mLastInventory;

  public:
  virtual bool IsUniversalCrafter() const override;

  virtual void SelectRecipeIndex(APlayerController *pc, int32 i) override;

  virtual void LoadSettings(TSharedPtr<FJsonObject> json, AMainPlayerController *mpc = nullptr) override;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  void PrepareInventories() const;

  virtual void BlockBeginPlay() override;

  virtual void CopyOnReplace(UBlockLogic *from) override;

  int32 cap_replace = 0;
  bool recipe_prepared = false;
};