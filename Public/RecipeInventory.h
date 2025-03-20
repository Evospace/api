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

  public:
  virtual void SortKeyAZ() override;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  TArray<FRecipeItemData> mRecipeData;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  URecipe *mRecipe;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  virtual TSubclassOf<UBaseInventoryWidgetBase> GetWidgetClass() const override;
};