// Copyright (c) 2017 - 2026, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "Blueprint/UserWidget.h"
#include "LogicResultsListWidget.generated.h"

class UButton;
class ULogicInterfaceBlockLogic;
class ULogicNode_Decider;
class ULogicOutput;
class ULogicResultRowWidget;
class UScrollBox;
class UVerticalBox;

/**
 * Results editor panel for ULogicNode_Decider::Output.
 * Embed in LogicCircuitWidget, keep a named reference, call SetupFromLogicBlock
 * from parent Event Construct after Super (same as LogicImportSwitchesPanelWidget::Setup).
 */
UCLASS(BlueprintType, meta = (DisplayName = "Logic Results Panel"))
class ULogicResultsListWidget : public UUserWidget {
  GENERATED_BODY()

  protected:
  virtual void NativeOnInitialized() override;

  public:
  UFUNCTION(BlueprintCallable, Category = "Logic|Results")
  void Setup(ULogicNode_Decider *Decider);

  UFUNCTION(BlueprintCallable, Category = "Logic|Results")
  void SetupFromLogicBlock(ULogicInterfaceBlockLogic *LogicBlock);

  UFUNCTION(BlueprintCallable, Category = "Logic|Results")
  void RebuildList();

  UPROPERTY(BlueprintReadOnly, Category = "Logic|Results")
  TObjectPtr<ULogicNode_Decider> BoundDecider;

  private:
  void BuildLayout();

  UFUNCTION()
  void HandleAddClicked();

  UPROPERTY()
  TObjectPtr<UVerticalBox> RootBox;

  UPROPERTY()
  TObjectPtr<UButton> AddButton;

  UPROPERTY()
  TObjectPtr<UScrollBox> ResultsScroll;
};
