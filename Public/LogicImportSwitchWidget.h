// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "Blueprint/UserWidget.h"
#include "Components/CheckBox.h"
#include "LogicImportOption.h"
#include "LogicImportSwitchWidget.generated.h"

class ULogicSettings;
class UEvoTextWidget;
class UBaseInventoryWidgetBase;

/**
 * Single import toggle row: EvoText label + checkbox bound to ULogicSettings::ImportEnabled at OptionIndex.
 * Root should stay visually empty (no panel image) in UMG if you want a chrome-free row.
 */
UCLASS(BlueprintType, meta = (DisplayName = "Logic Import Switch"))
class ULogicImportSwitchWidget : public UUserWidget {
  GENERATED_BODY()

  public:
  /**
   * Bind to a logic block's settings and one import option index. Call from Blueprint after the widget is built.
   * After a successful bind, OnImportRowInitialized runs (safe to read BoundImportOption there; do not use Construct).
   */
  UFUNCTION(BlueprintCallable, Category = "Logic|Import")
  void Setup(ULogicSettings *LogicSettings, int32 OptionIndex);

  /** Re-read label, tooltip, and checkbox state from BoundLogicSettings / GetImportSignals(). */
  UFUNCTION(BlueprintCallable, Category = "Logic|Import")
  bool RefreshFromSettings();

  /** Called once after Setup() completes successfully (BoundImportOption and inventories are ready). */
  UFUNCTION(BlueprintImplementableEvent, Category = "Logic|Import", meta = (DisplayName = "On Import Row Initialized"))
  void OnImportRowInitialized();

  UFUNCTION(BlueprintPure, Category = "Logic|Import")
  bool IsImportEnabled() const;

  UPROPERTY(BlueprintReadOnly, Category = "Logic|Import")
  TObjectPtr<ULogicSettings> BoundLogicSettings;

  UPROPERTY(BlueprintReadOnly, Category = "Logic|Import")
  int32 OptionIndex = INDEX_NONE;

  /** Prototype row from StaticBlock.ImportOptions (same as GetImportSignals()[OptionIndex]). */
  UPROPERTY(BlueprintReadOnly, Category = "Logic|Import")
  TObjectPtr<ULogicImportOption> BoundImportOption;

  UPROPERTY(meta = (BindWidget))
  TObjectPtr<UEvoTextWidget> ImportLabel;

  UPROPERTY(meta = (BindWidget))
  TObjectPtr<UCheckBox> ImportCheck;

  // Inventory-based UI element for showing import signal item.
  UPROPERTY(meta = (BindWidget))
  TObjectPtr<UBaseInventoryWidgetBase> ImportSignal;

  private:
  UFUNCTION()
  void OnImportCheckStateChanged(bool bIsChecked);

  bool bSuppressImportCallback = false;
};
