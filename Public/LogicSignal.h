// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Qr/CommonConverter.h"
#include "Qr/Loc.h"
#include "Condition.h"
#include "LogicExportOption.h"
#include "LogicSignal.generated.h"

UCLASS(BlueprintType)
class ULogicSignal : public UInstance {
  GENERATED_BODY()
  using Self = ULogicSignal;
  EVO_CODEGEN_INSTANCE(LogicSignal);
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UInstance>("LogicSignal") //@class LogicSignal : Instance
      .addProperty("export", QR_ARRAY_GET_SET(ExportSignals)) //@field LogicExportOption[]
      .addProperty("import", QR_ARRAY_GET_SET(ImportSignals)) //@field LogicExportOption[]
      .endClass();
  }

  public:
  // Signals available in UI for export/indication (metadata only)
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<ULogicExportOption *> ExportSignals;

  // Signals available in UI for input/indication (metadata only)
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<ULogicExportOption *> ImportSignals;

  // Per-instance export enabled flags; length matches ExportSignals
  // Do not mutate ULogicExportOption::bEnabled at runtime – use these flags instead
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<bool> ExportEnabled;

  // Initialize ExportEnabled from default options if sizes mismatch or empty
  UFUNCTION(BlueprintCallable)
  void EnsureExportFlagsInitialized();

  UFUNCTION(BlueprintCallable, BlueprintPure)
  bool IsExportEnabled(int32 Index = 0) const;

  UFUNCTION(BlueprintCallable)
  void SetExportEnabled(int32 Index, bool bEnabled);

  UFUNCTION(BlueprintCallable, BlueprintPure)
  bool IsModified() const;

  // Serialization for saving per-block state
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  // Performance optimization: Cache for inventory signals
  // This prevents unnecessary recalculations when inventory hasn't changed
  struct FInventoryCache {
    TArray<FItemData> CachedSlots;
    int32 LastInventoryHash = 0;
    bool bIsValid = false;

    // Calculate hash of inventory contents for change detection
    int32 CalculateInventoryHash(const TArray<FItemData> &Slots) const;

    // Check if inventory has changed since last cache
    bool HasInventoryChanged(const TArray<FItemData> &Slots) const;

    // Update cache with new inventory data
    void UpdateCache(const TArray<FItemData> &Slots);

    // Clear cache when inventory is modified
    void InvalidateCache();
  };

  // Cache for each export signal index
  mutable TArray<FInventoryCache> ExportCaches;

  // Get cached inventory data for export signal
  const TArray<FItemData> &GetCachedInventoryData(int32 ExportIndex, const TArray<FItemData> &CurrentSlots) const;

  // Invalidate all caches (call when inventory changes)
  void InvalidateAllCaches();
};
