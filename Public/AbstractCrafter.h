// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "BlockState.h"
#include "Qr/Prototype.h"
#include "Evospace/Blocks/ItemHopperInterface.h"
#include "Public/TieredBlockLogic.h"

#include "AbstractCrafter.generated.h"

class UResourceInventory;
class UResourceComponent;
class URecipe;
class URecipeDictionary;
class UAutosizeInventory;
class UInventoryContainer;

UCLASS(Blueprintable, BlueprintType)
class EVOSPACE_API UAbstractCrafter : public UBlockLogic, public ISwitchInterface {
  using Self = UAbstractCrafter;
  GENERATED_BODY()
  EVO_CODEGEN_INSTANCE(AbstractCrafter);
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UBlockLogic>("AbstractCrafter") //@class AbstractCrafter : BlockLogic
      .addProperty("recipes", &Self::RecipeDictionary) //@field RecipeDictionary
      .addProperty("load_independent", &Self::LoadIndependent) //@field boolean
      .addProperty("stable_supply", &Self::StableSupply) //@field boolean
      .addProperty("input_gathered", &Self::InputGathered) //@field boolean
      .addProperty("switch_on", &Self::SwitchedOn) //@field boolean
      .addProperty("ticks_passed", &Self::TicksPassed) //@field integer
      .addProperty("real_ticks_passed", &Self::RealTicksPassed) //@field integer
      .addProperty("total_production", &Self::TotalProduction) //@field integer
      .addProperty("speed", &Self::Speed) //@field integer

      .addProperty("energy_input_inventory", &Self::EnergyInputInventory) //@field ResourceInventory
      .addProperty("energy_output_inventory", &Self::EnergyOutputInventory) //@field ResourceInventory

      .addProperty("crafter_input_container", &Self::CrafterInputContainer) //@field InventoryContainer
      .addProperty("crafter_output_container", &Self::CrafterOutputContainer) //@field InventoryContainer
      .endClass();
  }

  public:
  UAbstractCrafter();

  //
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;

  UFUNCTION(BlueprintCallable)
  virtual URecipeDictionary *GetRecipeDictionary() const;

  UFUNCTION(BlueprintCallable)
  virtual URecipe *GetCurrentRecipe() const;

  UFUNCTION(BlueprintCallable)
  virtual void SelectRecipeIndex(APlayerController *pc, int32 i);

  UFUNCTION(BlueprintCallable)
  virtual void SelectRecipe(APlayerController *pc, const URecipe *recipe);

  UFUNCTION(BlueprintCallable)
  virtual void FindRecipe(APlayerController *pc, const UStaticItem *i);

  UFUNCTION(BlueprintCallable)
  virtual void ResetRecipe();

  UFUNCTION(BlueprintCallable)
  FText GetDescription() const;

  UFUNCTION(BlueprintCallable, BlueprintCosmetic)
  virtual float GetCurrentProgress() const;

  UFUNCTION(BlueprintCallable)
  virtual int32 GetProductivity() const;

  UFUNCTION(BlueprintCallable, BlueprintCosmetic)
  virtual float GetProductivityProgress() const;

  UPROPERTY(BlueprintReadOnly)
  bool LockedInput = false;

  virtual void OnCraftPerformed();

  virtual void BlockEndPlay() override;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UInventoryContainer *CrafterInputContainer = nullptr;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UInventoryContainer *CrafterOutputContainer = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  UResourceInventory *EnergyInputInventory = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  UResourceInventory *EnergyOutputInventory = nullptr;

  TArray<class UResourceAccessor *> GetResourceInputAccessors() const;
  TArray<class UResourceAccessor *> GetResourceOutputAccessors() const;

  virtual bool IsBlockTicks() const override;

  UFUNCTION(BlueprintCallable)
  virtual bool IsUniversalCrafter() const;

  virtual void SetSwitch_Implementation(bool val) override;
  virtual bool GetSwitch_Implementation() const override;

  virtual void CopyOnReplace(UBlockLogic *from) override;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  bool LoadIndependent = false;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  bool StableSupply = true;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  bool InputGathered = false;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  bool SwitchedOn = true;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  int32 TicksPassed = 0;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  int32 RealTicksPassed = 0;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  int32 TotalProduction = 0;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  int32 Speed = 100;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  EBlockState State = EBlockState::InputShortage;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  int32 InputTicks = 0;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  int32 OutputTicks = 0;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  int32 CollectedProductivity = 0;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  URecipeDictionary *RecipeDictionary;

  friend class UBlockModdingLuaState;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  const URecipe *CurrentRecipe = nullptr;
};