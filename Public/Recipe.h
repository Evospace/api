// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once
#include "ItemData.h"
#include "Evospace/ISearchable.h"
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
      .deriveClass<URecipe, UPrototype>("Recipe")
      .addProperty("ticks", &URecipe::mDuration)
      .addProperty("default_locked", &URecipe::mDefaultLocked, false)
      .addProperty("locked", &URecipe::mLocked, false)
      .addProperty("productivity", &URecipe::mProductivityBonus)
      .addProperty("input", &URecipe::mInput, false)
      .addProperty("output", &URecipe::mOutput, false)
      .addProperty("tier", &URecipe::Tier)
      .addProperty("start_tier", &URecipe::StartTier)
      .endClass();
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
  int32 Tier = 0;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  mutable int32 StartTier = 0;

  virtual void Release() override;
};