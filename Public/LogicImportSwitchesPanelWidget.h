// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "Blueprint/UserWidget.h"
#include "Public/LogicImportSwitchWidget.h"
#include "LogicImportSwitchesPanelWidget.generated.h"

class ULogicSettings;
class UScrollBox;

/**
 * Scroll list of import toggles for one block. Spawns one ULogicImportSwitchWidget per entry in
 * owning block StaticBlock.ImportOptions (via LogicSettings::GetImportSignals()). Set DefaultImportSwitchRowClass on a Blueprint child, or pass RowClass to Setup.
 */
UCLASS(BlueprintType, meta = (DisplayName = "Logic Import Switches Panel"))
class ULogicImportSwitchesPanelWidget : public UUserWidget {
  GENERATED_BODY()

  public:
  /**
   * Row widget class used when Setup's RowClass is not set. Assign in a Blueprint subclass of this panel.
   */
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Logic|Import")
  TSubclassOf<ULogicImportSwitchWidget> DefaultImportSwitchRowClass;

  /**
   * Rebuilds the scroll contents and binds each row. RowClass may be null to use DefaultImportSwitchRowClass.
   */
  UFUNCTION(BlueprintCallable, Category = "Logic|Import")
  void Setup(ULogicSettings *LogicSettings, TSubclassOf<ULogicImportSwitchWidget> RowClass);

  /** Calls RefreshFromSettings on every row widget in the scroll box. */
  UFUNCTION(BlueprintCallable, Category = "Logic|Import")
  void RefreshAllRows();

  UPROPERTY(BlueprintReadOnly, Category = "Logic|Import")
  TObjectPtr<ULogicSettings> BoundLogicSettings;

  UPROPERTY(meta = (BindWidget))
  TObjectPtr<UScrollBox> ImportScroll;
};
