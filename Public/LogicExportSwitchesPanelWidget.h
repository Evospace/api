// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "Blueprint/UserWidget.h"
#include "Public/LogicExportSwitchWidget.h"
#include "LogicExportSwitchesPanelWidget.generated.h"

class ULogicSettings;
class UScrollBox;

/**
 * Scroll list of export toggles for one block. Spawns one ULogicExportSwitchWidget per entry in
 * LogicSettings::ExportSignals. Set DefaultExportSwitchRowClass on a Blueprint child, or pass RowClass to Setup.
 */
UCLASS(BlueprintType, meta = (DisplayName = "Logic Export Switches Panel"))
class ULogicExportSwitchesPanelWidget : public UUserWidget {
  GENERATED_BODY()

  public:
  /**
   * Row widget class used when Setup's RowClass is not set. Assign in a Blueprint subclass of this panel.
   */
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Logic|Export")
  TSubclassOf<ULogicExportSwitchWidget> DefaultExportSwitchRowClass;

  /**
   * Rebuilds the scroll contents and binds each row. RowClass may be null to use DefaultExportSwitchRowClass.
   */
  UFUNCTION(BlueprintCallable, Category = "Logic|Export")
  void Setup(ULogicSettings *LogicSettings, TSubclassOf<ULogicExportSwitchWidget> RowClass);

  /** Calls RefreshFromSettings on every row widget in the scroll box. */
  UFUNCTION(BlueprintCallable, Category = "Logic|Export")
  void RefreshAllRows();

  UPROPERTY(BlueprintReadOnly, Category = "Logic|Export")
  TObjectPtr<ULogicSettings> BoundLogicSettings;

  UPROPERTY(meta = (BindWidget))
  TObjectPtr<UScrollBox> ExportScroll;
};
