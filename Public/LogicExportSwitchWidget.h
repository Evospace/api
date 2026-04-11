// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "Blueprint/UserWidget.h"
#include "Components/CheckBox.h"
#include "LogicExportSwitchWidget.generated.h"

class ULogicSettings;
class UEvoTextWidget;

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
   */
  UFUNCTION(BlueprintCallable, Category = "Logic|Export")
  void Setup(ULogicSettings *LogicSettings, int32 OptionIndex);

  /** Re-read label, tooltip, and checkbox state from BoundLogicSettings / ExportSignals. */
  UFUNCTION(BlueprintCallable, Category = "Logic|Export")
  void RefreshFromSettings();

  UFUNCTION(BlueprintPure, Category = "Logic|Export")
  bool IsExportEnabled() const;

  UPROPERTY(BlueprintReadOnly, Category = "Logic|Export")
  TObjectPtr<ULogicSettings> BoundLogicSettings;

  UPROPERTY(BlueprintReadOnly, Category = "Logic|Export")
  int32 OptionIndex = INDEX_NONE;

  UPROPERTY(meta = (BindWidget))
  TObjectPtr<UEvoTextWidget> ExportLabel;

  UPROPERTY(meta = (BindWidget))
  TObjectPtr<UCheckBox> ExportCheck;

  private:
  UFUNCTION()
  void OnExportCheckStateChanged(bool bIsChecked);

  bool bSuppressExportCallback = false;
};
