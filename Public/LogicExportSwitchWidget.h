// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "Blueprint/UserWidget.h"
#include "Components/CheckBox.h"
#include "LogicExportOption.h"
#include "LogicExportSwitchWidget.generated.h"

class ULogicSettings;
class UEvoTextWidget;
class UBaseInventoryWidgetBase;

/**
 * Single export toggle row: EvoText label + checkbox bound to ULogicSettings::ExportEnabled at OptionIndex.
 * Root should stay visually empty (no panel image) in UMG if you want a chrome-free row.
 */
UCLASS(BlueprintType, meta = (DisplayName = "Logic Export Switch"))
class ULogicExportSwitchWidget : public UUserWidget {
  GENERATED_BODY()

  public:
  /**
   * Bind to a logic block's settings and one export option index. Call from Blueprint after the widget is built.
   * After a successful bind, OnExportRowInitialized runs (safe to read BoundExportOption there; do not use Construct).
   */
  UFUNCTION(BlueprintCallable, Category = "Logic|Export")
  void Setup(ULogicSettings *LogicSettings, int32 OptionIndex);

  /** Re-read label, tooltip, and checkbox state from BoundLogicSettings / GetExportSignals(). */
  UFUNCTION(BlueprintCallable, Category = "Logic|Export")
  bool RefreshFromSettings();

  /** Called once after Setup() completes successfully (BoundExportOption and inventories are ready). */
  UFUNCTION(BlueprintImplementableEvent, Category = "Logic|Export", meta = (DisplayName = "On Export Row Initialized"))
  void OnExportRowInitialized();

  UFUNCTION(BlueprintPure, Category = "Logic|Export")
  bool IsExportEnabled() const;

  UPROPERTY(BlueprintReadOnly, Category = "Logic|Export")
  TObjectPtr<ULogicSettings> BoundLogicSettings;

  UPROPERTY(BlueprintReadOnly, Category = "Logic|Export")
  int32 OptionIndex = INDEX_NONE;

  /** Prototype row from StaticBlock.ExportOptions (same as GetExportSignals()[OptionIndex]). */
  UPROPERTY(BlueprintReadOnly, Category = "Logic|Export")
  TObjectPtr<ULogicExportOption> BoundExportOption;

  UPROPERTY(meta = (BindWidget))
  TObjectPtr<UEvoTextWidget> ExportLabel;

  UPROPERTY(meta = (BindWidget))
  TObjectPtr<UCheckBox> ExportCheck;

  // Inventory-based UI element for showing export signal item.
  UPROPERTY(meta = (BindWidget))
  TObjectPtr<UBaseInventoryWidgetBase> ExportSignal;

  private:
  UFUNCTION()
  void OnExportCheckStateChanged(bool bIsChecked);

  bool bSuppressExportCallback = false;
};
