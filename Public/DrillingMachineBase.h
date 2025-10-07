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
class UDrillingMachineBase : public UBlockLogic {
  GENERATED_BODY()
  using Self = UDrillingMachineBase;
  EVO_CODEGEN_INSTANCE(DrillingMachineBase)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UBlockLogic>("DrillingMachineBase") //@class DrillingMachineBase : BlockLogic
      .addProperty("ticks_per_item", &Self::TicksPerItem) //@field integer
      .addProperty("energy_per_tick", &Self::EnergyPerTick) //@field integer
      .addProperty("remaining_energy", &Self::RemainingEnergy) //@field integer
      .addProperty("productivity", &Self::Productivity) //@field integer percent (e.g. 15 = +15%)
      .addFunction("has_storage_space", &Self::HasStorageSpace) //@function boolean
      .addFunction("is_energy_available", &Self::IsEnergyAvailable) //@function boolean
      .addProperty("inventory", &Self::Inventory) //@field InventoryContainer
      .addProperty("energy", &Self::Energy) //@field ResourceInventory
      .addProperty("production", &Self::Production) //@field integer
      .addProperty("storage_size", &Self::StorageSize) //@field integer
      .endClass();
  }

  public:
  UDrillingMachineBase();
  virtual UCoreAccessor *CoreInit() override;

  UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintCosmetic, Category = "Drilling|Energy")
  float GetConsumption() const;

  UFUNCTION(BlueprintCallable, BlueprintCosmetic, BlueprintPure, Category = "Drilling|Energy")
  float CurrentUsage() const;

  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Drilling|Stats")
  virtual float GetMiningProgress() const;

  // Bonus (productivity) progress in [0..1]
  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Drilling|Stats")
  virtual float GetBonusMiningProgress() const;

  int32 Production = 1;

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
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) const override;
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

  // Exact energy consumption per tick
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drilling|Energy", meta = (ClampMin = "0"))
  int32 EnergyPerTick = 60;

  // Exact time to extract one item, in ticks
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drilling|Energy", meta = (ClampMin = "1"))
  int32 TicksPerItem = 100;

  // Storage configuration
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drilling|Storage", meta = (ClampMin = "1"))
  int32 StorageSize = 100;

  // Mining productivity percent (e.g., 15 means +15% output)
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drilling|Stats", meta = (ClampMin = "0"))
  int32 Productivity = 0;

  // Accumulated productivity points for bonus progress
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drilling|Stats")
  int32 CollectedProductivity = 0;

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