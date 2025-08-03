// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once
#include "BlockLogic.h"
#include "EvoRingBuffer.h"
#include "DrillingMachineBase.generated.h"

class UInventoryContainer;
class UResourceAccessor;
class USingleSlotInventory;
class USolidOutputAccessor;

/**
 * Base class for all drilling machines in the game.
 * Handles resource extraction, energy consumption, and inventory management.
 */
UCLASS(Abstract)
class EVOSPACE_API UDrillingMachineBase : public UBlockLogic {
  GENERATED_BODY()
  using Self = UDrillingMachineBase;
  EVO_CODEGEN_INSTANCE(DrillingMachineBase)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UBlockLogic>("DrillingMachineBase") //@class DrillingMachineBase : BlockLogic
      .addProperty("energy_per_one", &Self::EnergyPerOne) //@field integer
      .addProperty("remaining_energy", &Self::RemainingEnergy) //@field integer
      .addFunction("get_speed", &Self::GetSpeed) //@function number
      .addFunction("get_consumption", &Self::GetConsumption) //@function number
      .addFunction("get_storage_fullness", &Self::GetStorageFullness) //@function number
      .addFunction("has_storage_space", &Self::HasStorageSpace) //@function boolean
      .addFunction("is_energy_available", &Self::IsEnergyAvailable) //@function boolean
      .addProperty("inventory", &Self::Inventory) //@field InventoryContainer
      .addProperty("energy", &Self::Energy) //@field ResourceInventory
      .addProperty("production", &Self::Production) //@field integer
      .endClass();
  }

  public:
  UDrillingMachineBase();

  // Blueprint interface
  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Drilling|Stats")
  float GetSpeed() const;

  UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintCosmetic, Category = "Drilling|Energy")
  float GetConsumption() const;

  UFUNCTION(BlueprintCallable, BlueprintCosmetic, BlueprintPure, Category = "Drilling|Energy")
  float CurrentUsage() const;

  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Drilling|Stats")
  virtual float GetMiningProgress() const;

  int32 Production = 1;

  UFUNCTION(BlueprintCallable, Category = "Drilling|Inventory")
  float GetStorageFullness() const;

  UFUNCTION(BlueprintCallable)
  virtual TArray<UStaticItem *> GetExtractOption() const;

  UFUNCTION(BlueprintCallable)
  virtual UStaticItem *GetSelectedOption() const;

  UFUNCTION(BlueprintCallable)
  virtual void SetExtractOption(UStaticItem *item);

  // UBlockLogic interface
  virtual void Tick() override;

  virtual void BlockBeginPlay() override;
  virtual bool IsBlockTicks() const override { return true; }
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  protected:
  // Core components
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drilling|Components")
  UInventoryContainer *Inventory;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drilling|Components")
  class UResourceInventory *Energy;

  // Energy management
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drilling|Energy", meta = (ClampMin = "0"))
  int32 RemainingEnergy = 0;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drilling|Energy", meta = (ClampMin = "1"))
  int32 EnergyPerOne = 600 * 8;

  // Storage configuration
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drilling|Storage", meta = (ClampMin = "1"))
  int32 StorageSize = 100;

  // Utility functions
  virtual void ProcessEnergy();

  virtual void Drill() PURE_VIRTUAL(UDrillingMachineBase::Drill, );
  bool IsEnergyAvailable() const { return RemainingEnergy > 0; }
  bool HasStorageSpace() const;

  protected:
  static float CalcMiningProgress(int32 RemainingEnergy, float maxEnergy) {
    if (maxEnergy <= 0.0f) return 0.0f;
    return 1.0f - FMath::Clamp(RemainingEnergy / maxEnergy, 0.0f, 1.0f);
  }

  private:
  static constexpr float basic_duration_tick = 20;
  EvoRingBuffer<int32> SupportRing = EvoRingBuffer<int32>(60);
};