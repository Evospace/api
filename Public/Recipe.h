// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once
#include "ItemData.h"
#include "Evospace/ISearchable.h"
#include "Evospace/Misc/AssetOwner.h"
#include "Public/Prototype.h"

#include <Dom/JsonObject.h>

#include "Recipe.generated.h"

class URecipeDictionary;
class URecipeInventory;
class UStaticResearchRecipe;

UCLASS(BlueprintType)
class EVOSPACE_API URecipe : public UPrototype, public ISearchable {
  GENERATED_BODY()
  EVO_CODEGEN_DB(Recipe, Recipe)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      //@comment Crafting-recipe prototype used by machines and crafting UIs
      .deriveClass<URecipe, UPrototype>("Recipe") //@class Recipe : Prototype
      .addProperty("ticks", &URecipe::mDuration) //@field integer *Craft time* in engine ticks
      .addProperty("default_locked", &URecipe::mDefaultLocked) //@field boolean If **true**, the recipe starts hidden until research unlocks it.
      .addProperty("locked", &URecipe::mLocked) //@field boolean Currently locked if **true**
      .addProperty("productivity", &URecipe::mProductivityBonus) //@field integer Percentage bonus (e.g. `20` = +20 %)
      .addProperty("input", &URecipe::mInput, false) //@field RecipeInventory Read/write container of required items
      .addProperty("output", &URecipe::mOutput, false) //@field RecipeInventory Read/write container of produced items
      .addProperty("tier", &URecipe::Tier) //@field integer Recipe tier used for speed scaling: every tier **above** the machine tier halves speed; every tier **below** doubles it.
      .addProperty("start_tier", &URecipe::StartTier) //@field integer Machine-unlock tier (same for all its recipes); lets you compute the recipe’s level relative to that minimum.
      .endClass();
  }
  virtual UClass *GetSuperProto() const override {
    return StaticClass();
  }

  public:
  URecipe();

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  virtual void ComputeSearchMetadata() const override;

  UFUNCTION(BlueprintCallable)
  virtual float GetCostMultiplier(int32 tier = 0) const;

  UFUNCTION(BlueprintCallable)
  virtual float GetDurationMultiplier(int32 tier = 0) const;

  UFUNCTION(BlueprintCallable)
  virtual float GetConversionMultiplier(int32 tier = 0) const;

  UFUNCTION(BlueprintCallable)
  virtual float GetCostOrConversion(int32 tier = 0) const;

  UFUNCTION(BlueprintCallable, BlueprintCosmetic)
  class UTexture2D *GetTexture() const;

  UFUNCTION(BlueprintCallable, BlueprintCosmetic)
  FText GetLabel() const;

  UFUNCTION(BlueprintCallable, BlueprintCosmetic)
  int64 GetDuration() const;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  URecipeInventory *mInput;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  URecipeInventory *mOutput;

  UPROPERTY(EditAnywhere)
  int32 mDuration = 1;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  mutable bool mLocked = false;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  bool mDefaultLocked = false;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  int32 mProductivityBonus = 0;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UStaticResearchRecipe *mUnlocksBy;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  TArray<FLinearColor> Colors;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  URecipeDictionary *Dictionary;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  mutable int32 Tier = 0;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  mutable int32 StartTier = 0;

  virtual void Release() override;
};