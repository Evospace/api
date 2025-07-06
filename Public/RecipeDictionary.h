// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "Containers/Map.h"
#include "CoreMinimal.h"
#include "Qr/Prototype.h"
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
class EVOSPACE_API URecipeDictionary : public UPrototype, public ISearchable {
  GENERATED_BODY()
  PROTOTYPE_CODEGEN(RecipeDictionary, RecipeDictionary)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<URecipeDictionary, UPrototype>("RecipeDictionary") //@class RecipeDictionary : Prototype
      .addProperty("start_tier", &URecipeDictionary::StartTier) //@field integer
      .endClass();
  }
  virtual UClass *GetSuperProto() const override {
    return StaticClass();
  }

  public:
  URecipeDictionary();

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual void ModLoadFinalize() override;

  virtual void ComputeSearchMetadata() const;

  UFUNCTION(BlueprintCallable)
  void UpdatePerRecipeInventory();

  UFUNCTION(BlueprintCallable)
  static TArray<URecipe *> GetRecipesSorted(TArray<URecipe *> r);

  const URecipe *FindByInput(UInventoryReader *inventory, bool ignore_locked = false) const;

  const TArray<const URecipe *> &GetRecipes() const;

  const TArray<const UStaticItem *> &GetUsedIn() const;

  URecipe *GetByName(FName name) const;

  //   ,
  class UNeiCrafterInventory *GetUsedInInventory();

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
  UNeiCrafterInventory *mUsedInInventory;

  // item per every recipe in this dictionary
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  UInventory *mItemPerRecipeInventory;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  UInventory *mAvailibleItemPerRecipeInventory;
};