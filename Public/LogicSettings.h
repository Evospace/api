// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Qr/Loc.h"
#include "Condition.h"
#include "LogicExportOption.h"
#include "LogicImportOption.h"
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
  // Export/import option metadata comes from owning UBlockLogic::GetStaticBlock()
  // (see GetExportSignals/GetImportSignals).

  // Per-instance export enabled flags; length matches GetExportSignals()
  // Do not mutate ULogicExportOption::bEnabled at runtime – use these flags instead
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<bool> ExportEnabled;

  // Per-instance import enabled flags; length matches GetImportSignals()
  // Block logic may use these flags to decide which input signals are active
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<bool> ImportEnabled;

  // Per-instance inventory slots for export signal pointers in GUI
  UPROPERTY(Transient)
  TArray<TObjectPtr<class UNonSerializedSingleSlotInventory>> ExportSignalInventories;

  // Per-instance inventory slots for import signal pointers in GUI
  UPROPERTY(Transient)
  TArray<TObjectPtr<class UNonSerializedSingleSlotInventory>> ImportSignalInventories;

  private:
  UPROPERTY(Transient)
  TObjectPtr<class ULogicContext> DeliveredInputContext;

  public:

  /** Prototype export list from the owning block's UStaticBlock (same as StaticBlock.export_options). */
  const TArray<ULogicExportOption *> &GetExportSignals() const;
  /** Prototype import list from the owning block's UStaticBlock (same as StaticBlock.import_options). */
  const TArray<ULogicImportOption *> &GetImportSignals() const;

  // Resize ExportEnabled to match export signals: keep existing prefix, new indices from prototype defaults
  UFUNCTION(BlueprintCallable)
  void EnsureExportFlagsInitialized();

  // Initialize and synchronize per-export signal pointer inventories from prototype options
  UFUNCTION(BlueprintCallable)
  void EnsureExportSignalInventoriesInitialized();

  // Initialize and synchronize per-import signal pointer inventories from prototype options
  UFUNCTION(BlueprintCallable)
  void EnsureImportSignalInventoriesInitialized();

  UFUNCTION(BlueprintCallable, BlueprintPure)
  class UNonSerializedSingleSlotInventory *GetExportSignalInventory(int32 Index) const;

  UFUNCTION(BlueprintCallable, BlueprintPure)
  class UNonSerializedSingleSlotInventory *GetImportSignalInventory(int32 Index) const;

  // Effective signal: inventory slot override > prototype option fallback
  UFUNCTION(BlueprintCallable, BlueprintPure)
  const UStaticItem *GetResolvedExportSignal(int32 Index) const;

  UFUNCTION(BlueprintCallable, BlueprintPure)
  const UStaticItem *GetResolvedImportSignal(int32 Index) const;

  UFUNCTION(BlueprintCallable, BlueprintPure)
  bool IsExportEnabled(int32 Index = 0) const;

  UFUNCTION(BlueprintCallable)
  void SetExportEnabled(int32 Index, bool bEnabled);

  // Resize ImportEnabled to match import signals: keep existing prefix, new indices from prototype defaults
  UFUNCTION(BlueprintCallable)
  void EnsureImportFlagsInitialized();

  UFUNCTION(BlueprintCallable, BlueprintPure)
  bool IsImportEnabled(int32 Index = 0) const;

  UFUNCTION(BlueprintCallable)
  void SetImportEnabled(int32 Index, bool bEnabled);

  // Last logic input delivered to the owning block (data wire or direct push).
  // Created on first GUI request; transport records into it only after that,
  // so blocks whose settings GUI was never opened pay nothing.
  UFUNCTION(BlueprintCallable)
  class ULogicContext *GetDeliveredInputContext();

  void RecordDeliveredInput(const class UItemMap *SourceInput);

  // Serialization for saving per-block state
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) const override;
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
};
