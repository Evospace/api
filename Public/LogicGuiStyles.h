// Copyright (c) 2017 - 2026, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

class UBorder;
class UButton;
class UComboBoxString;
class USpinBox;
class UTextBlock;

/** Project chrome for logic circuit C++ panels (matches LogicCircuitWidget / OutputBlock). */
namespace LogicGuiStyles {
void ApplyPanelButtonStyle(UButton *button);
void ApplyComboBoxStyle(UComboBoxString *combo);
void ApplySpinBoxStyle(USpinBox *spin);
/** Accent text: combo items, button glyphs. */
void ApplyPanelTextStyle(UTextBlock *text);
/** Muted field labels (Value, Source, …). */
void ApplyPanelLabelStyle(UTextBlock *text);
/** Secondary hint copy. */
void ApplyPanelHintStyle(UTextBlock *text);
void ApplyPanelRowBorderStyle(UBorder *border);
} // namespace LogicGuiStyles
