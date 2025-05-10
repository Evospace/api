// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once
#include "AutosizeInventory.h"
#include "Containers/Array.h"
#include "Evospace/Item/InventoryWidget.h"
#include "Public/Recipe.h"

#include <Dom/JsonObject.h>
#include <Templates/SharedPointer.h>
#include <Templates/SubclassOf.h>

#include "RecipeInventory.generated.h"

class URecipe;

UCLASS(BlueprintType)
class EVOSPACE_API URecipeInventory : public UAutosizeInventory {
  GENERATED_BODY()
  EVO_CODEGEN_INSTANCE(RecipeInventory)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<URecipeInventory, UInventory>("RecipeInventory") //@class RecipeInventory : AutosizeInventory
      .addProperty("recipe", &URecipeInventory::mRecipe) //@field Recipe
      .endClass();
  }

  public:
  virtual void SortKeyAZ() override;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  TArray<FRecipeItemData> mRecipeData;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  URecipe *mRecipe;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  virtual TSubclassOf<UBaseInventoryWidgetBase> GetWidgetClass() const override;
};