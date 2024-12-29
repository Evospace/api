// Copyright (c) 2017 - 2024, Samsonov Andrey. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Prototype.h"
#include "Evospace/Blocks/ItemHopperInterface.h"
#include "Public/TieredBlockLogic.h"

#include "AbstractCrafter.generated.h"

class UResourceInventory;
class UResourceComponent;
class URecipe;
class URecipeDictionary;
class UAutosizeInventory;
class UInventoryContainer;

UENUM(BlueprintType)
enum class ECrafterState : uint8 {
  InputShortage,
  Working,
  NoRoomInOutput,
  ResourceSaturated,
  ResourceRequired,
  NotInitialized,
  SwitchedOff,
};

UCLASS(BlueprintType)
/**
 * 
 */
class EVOSPACE_API UAbstractCrafter : public UBlockLogic, public ISwitchInterface {
  using Self = UAbstractCrafter;
  GENERATED_BODY()
  EVO_CODEGEN_INSTANCE(AbstractCrafter);
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UBlockLogic>("AbstractCrafter") //class: AbstractCrafter, parent: BlockLogic
      .addProperty("recipes", &Self::RecipeDictionary) //field: RecipeDictionary
      .addProperty("load_independent", &Self::LoadIndependent) //field: boolean
      .addProperty("input_gathered", &Self::InputGathered) //field: boolean
      .addProperty("switched_on", &Self::SwitchedOn) //field: boolean
      .addProperty("ticks_passed", &Self::TicksPassed) //field: integer
      .addProperty("real_ticks_passed", &Self::RealTicksPassed) //field: integer
      .addProperty("total_production", &Self::TotalProduction) //field: integer
      .addProperty("speed", &Self::Speed) //field: integer

      .addProperty("energy_input_inventory", &Self::EnergyInputInventory) //field: ResourceInventory
      .addProperty("energy_output_inventory", &Self::EnergyOutputInventory) //field: ResourceInventory

      .addProperty("crafter_input_container", &Self::CrafterInputContainer) //field: ResourceInventory
      .addProperty("crafter_output_container", &Self::EnergyOutputInventory) //field: ResourceInventory
      .endClass();
  }

  public:
  UAbstractCrafter();

  UFUNCTION(BlueprintCallable, BlueprintCosmetic)
  FColor GetStateColor() const;

  virtual void SetActor(ABlockActor *actor) override;

  virtual void SetWorking(bool working);

  FBoolProperty *mWorking = nullptr;

  //
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;

  UFUNCTION(BlueprintCallable)
  virtual URecipeDictionary *GetRecipeDictionary() const;

  UFUNCTION(BlueprintCallable)
  virtual URecipe *GetCurrentRecipe() const;

  UFUNCTION(BlueprintCallable)
  virtual void SelectRecipe(APlayerController *pc, int32 i);

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

  //TODO: do we need this?
  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  UAutosizeInventory *RecipeListTierHelper = nullptr;

  TArray<class UResourceAccessor *> GetResourceInputAccessors() const;
  TArray<class UResourceAccessor *> GetResourceOutputAccessors() const;

  virtual bool IsBlockTicks() const override;

  UFUNCTION(BlueprintCallable)
  virtual bool IsUniversalCrafter() const;

  void SetSwitched(bool val) override;
  bool GetSwitched() const override;

  virtual void CopyOnReplace(UBlockLogic *from) override;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  bool LoadIndependent = false;

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
  ECrafterState State = ECrafterState::InputShortage;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  int32 InputTicks = 0;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  int32 OutputTicks = 0;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  int32 CollectedProductivity = 0;

  UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
  URecipeDictionary *RecipeDictionary;

  friend class UBlockModdingLuaState;

  UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
  const URecipe *CurrentRecipe = nullptr;
};