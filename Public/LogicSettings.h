// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Qr/Loc.h"
#include "Condition.h"
#include "LogicExportOption.h"
#include "LogicSettings.generated.h"

UCLASS(BlueprintType)
class ULogicSettings : public UInstance {
  GENERATED_BODY()
  using Self = ULogicSettings;
  EVO_CODEGEN_INSTANCE(LogicSettings);
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UInstance>("LogicSettings") //@class LogicSettings : Instance
      .endClass();
  }

  public:
  // Export option metadata comes from owning UBlockLogic::GetStaticBlock()->ExportOptions (see GetExportSignals).

  // Per-instance export enabled flags; length matches GetExportSignals()
  // Do not mutate ULogicExportOption::bEnabled at runtime – use these flags instead
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<bool> ExportEnabled;

  // Per-instance import enabled flags; length matches GetExportSignals()
  // Block logic may use these flags to decide which input signals are active
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<bool> ImportEnabled;

  // Per-instance inventory slots for export signal pointers in GUI
  UPROPERTY(Transient)
  TArray<TObjectPtr<class UNonSerializedSingleSlotInventory>> ExportSignalInventories;

  /** Prototype export list from the owning block's UStaticBlock (same as StaticBlock.export_options). */
  const TArray<ULogicExportOption *> &GetExportSignals() const;

  // Initialize ExportEnabled from default options if sizes mismatch or empty
  UFUNCTION(BlueprintCallable)
  void EnsureExportFlagsInitialized();

  // Initialize and synchronize per-export signal pointer inventories from prototype options
  UFUNCTION(BlueprintCallable)
  void EnsureExportSignalInventoriesInitialized();

  UFUNCTION(BlueprintCallable, BlueprintPure)
  class UNonSerializedSingleSlotInventory *GetExportSignalInventory(int32 Index) const;

  UFUNCTION(BlueprintCallable, BlueprintPure)
  bool IsExportEnabled(int32 Index = 0) const;

  UFUNCTION(BlueprintCallable)
  void SetExportEnabled(int32 Index, bool bEnabled);

  // Initialize ImportEnabled from default options if sizes mismatch or empty
  UFUNCTION(BlueprintCallable)
  void EnsureImportFlagsInitialized();

  UFUNCTION(BlueprintCallable, BlueprintPure)
  bool IsImportEnabled(int32 Index = 0) const;

  UFUNCTION(BlueprintCallable)
  void SetImportEnabled(int32 Index, bool bEnabled);

  // Serialization for saving per-block state
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) const override;
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
};
