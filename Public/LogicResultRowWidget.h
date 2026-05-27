// Copyright (c) 2017 - 2026, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "Blueprint/UserWidget.h"
#include "LogicOutput.h"
#include "LogicResultRowWidget.generated.h"

class UBorder;
class UButton;
class UCheckBox;
class UComboBoxString;
class UHorizontalBox;
class ULogicResultsListWidget;
class ULogicNode_Decider;
class ULogicSignalSlotWidget;
class USpinBox;
class UTextBlock;
class UVerticalBox;
class UWidgetSwitcher;

UCLASS(BlueprintType)
class ULogicResultRowWidget : public UUserWidget {
  GENERATED_BODY()

  protected:
  virtual void NativeOnInitialized() override;
  virtual void NativeConstruct() override;

  public:
  void Setup(ULogicResultsListWidget *Owner, ULogicNode_Decider *Decider, ULogicOutput *Output);

  private:
  void BuildLayout();
  void RefreshFromOutput();
  void ApplyToOutput();
  void RebuildBody();
  void EnsureFormulaTree();

  UFUNCTION()
  void HandleDeleteClicked();

  UFUNCTION()
  void HandleModeChanged(FString Selected, ESelectInfo::Type SelectionType);

  UFUNCTION()
  void HandleNumberChanged(float Value);

  UFUNCTION()
  void HandleExprOpChanged(FString Selected, ESelectInfo::Type SelectionType);

  UFUNCTION()
  void HandleRightConstToggled(bool bChecked);

  UFUNCTION()
  void HandleSignalSlotChanged();

  UFUNCTION()
  UWidget *HandleGenerateComboItem(FString Item);

  UPROPERTY()
  TObjectPtr<ULogicResultsListWidget> OwnerList;

  UPROPERTY()
  TObjectPtr<ULogicNode_Decider> BoundDecider;

  UPROPERTY()
  TObjectPtr<ULogicOutput> BoundOutput;

  UPROPERTY()
  TObjectPtr<UBorder> RootBorder;

  UPROPERTY()
  TObjectPtr<UHorizontalBox> HeaderRow;

  UPROPERTY()
  TObjectPtr<ULogicSignalSlotWidget> OutSignalSlot;

  UPROPERTY()
  TObjectPtr<UComboBoxString> ModeCombo;

  UPROPERTY()
  TObjectPtr<UButton> DeleteButton;

  UPROPERTY()
  TObjectPtr<UWidgetSwitcher> BodySwitcher;

  UPROPERTY()
  TObjectPtr<USpinBox> NumberSpin;

  UPROPERTY()
  TObjectPtr<ULogicSignalSlotWidget> SourceSignalSlot;

  UPROPERTY()
  TObjectPtr<UTextBlock> CurrentHint;

  UPROPERTY()
  TObjectPtr<UComboBoxString> ExprOpCombo;

  UPROPERTY()
  TObjectPtr<UCheckBox> RightConstCheck;

  UPROPERTY()
  TObjectPtr<USpinBox> RightNumberSpin;

  UPROPERTY()
  TObjectPtr<ULogicSignalSlotWidget> RightSignalSlot;

  bool bSuppressCallbacks = false;
};
