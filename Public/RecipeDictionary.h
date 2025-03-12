// Copyright (c) 2017 - 2024, Samsonov Andrey. All Rights Reserved.
#pragma once
#include "Containers/Map.h"
#include "CoreMinimal.h"
#include "Evospace/Misc/AssetOwner.h"
#include "Public/Prototype.h"
#include "Public/Recipe.h"

#include "RecipeDictionary.generated.h"
class UStaticItem;
class UInventory;
class URecipe;
class UInventoryReader;

// UENUM(BlueprintType)
// enum class EIODirection : uint8
//{
// };

UCLASS(BlueprintType)
class EVOSPACE_API URecipeDictionary : public UPrototype {
  GENERATED_BODY()
  EVO_OWNER(RecipeDictionary);
  EVO_CODEGEN_DB(RecipeDictionary, RecipeDictionary)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<URecipeDictionary, UPrototype>("RecipeDictionary") //@class RecipeDictionary : Prototype
      .endClass();
  }

  public:
  URecipeDictionary();

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual void PostDeserializeJson() override;

  UFUNCTION(BlueprintCallable)
  void UpdatePerRecipeInventory();

  UFUNCTION(BlueprintCallable)
  static TArray<URecipe *> GetRecipesSorted(TArray<URecipe *> r);

  const URecipe *FindByInput(UInventoryReader *inventory, bool ignore_locked = false) const;

  const TArray<const URecipe *> &GetRecipes() const;

  const TArray<const UStaticItem *> &GetUsedIn() const;

  URecipe *GetByName(const FName &name) const;

  //   ,
  UInventory *GetUsedInInventory();

  void ResetLocked();

  //TODO: remove default
  const URecipe *FindByOutput(const UStaticItem *item, int32 count = 1, bool ignore_locked = false) const;

  //TODO: remove default
  const URecipe *FindByOutput(UInventoryReader *inventory, bool ignore_locked = false) const;

  //TODO: remove default
  const URecipe *FindByItem(const UStaticItem *item, bool ignore_locked = false) const;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  int32 StartTier = 0;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  const UStaticItem *mUsedInInventoryFace;

  protected:
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  TArray<const URecipe *> mRecipes;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  TArray<const UStaticItem *> mUsedIn;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  TMap<FName, URecipe *> mNameChache;

  // item per every crafter that using this recipe dictionary
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  UInventory *mUsedInInventory;

  // item per every recipe in this dictionary
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  UInventory *mItemPerRecipeInventory;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  UInventory *mAvailibleItemPerRecipeInventory;
};