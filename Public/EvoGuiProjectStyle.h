// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "EvoGuiProjectStyle.generated.h"

class UTexture2D;

/**
 * Project-wide GUI defaults for mod/Lua UMG widgets (textures, fonts, button chrome).
 * Create an asset instance under Content (e.g. /Game/Gui/EvoGuiProjectStyle) and assign fields in the editor.
 */
UCLASS(BlueprintType)
class UEvoGuiProjectStyle : public UDataAsset {
  GENERATED_BODY()

  public:
  /** Normal / hovered chrome for mod buttons (rounded box). */
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button")
  TObjectPtr<UTexture2D> ButtonHighlightTexture;

  /** Pressed-state fill for mod buttons. */
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button")
  TObjectPtr<UTexture2D> ButtonHighlightFillTexture;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button|Rounded")
  float ButtonCornerRadius = 4.f;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button|Rounded")
  float ButtonOutlineWidth = 4.f;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button|Rounded")
  FLinearColor ButtonOutlineColor = FLinearColor(0.01f, 0.01f, 0.01f, 1.f);

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button|Rounded")
  FVector2D ButtonImageSize = FVector2D(32.f, 32.f);

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button|Rounded")
  FVector2D ButtonPressedImageSize = FVector2D(38.f, 38.f);

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button|Tint")
  FLinearColor ButtonNormalTint = FLinearColor::White;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button|Tint")
  FLinearColor ButtonHoveredTint = FLinearColor(0.447059f, 0.756863f, 1.f, 1.f);

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button|Tint")
  FLinearColor ButtonPressedTint = FLinearColor(0.025f, 0.025f, 0.025f, 1.f);

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button|Foreground")
  FSlateColor ButtonNormalForeground = FSlateColor(FLinearColor(0.f, 1.f, 1.f, 1.f));

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button|Foreground")
  FSlateColor ButtonHoveredForeground = FSlateColor(FLinearColor(0.f, 1.f, 1.f, 1.f));

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button|Foreground")
  FSlateColor ButtonPressedForeground = FSlateColor(FLinearColor(0.f, 1.f, 1.f, 1.f));

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button|Foreground")
  FSlateColor ButtonDisabledForeground = FSlateColor(FLinearColor(0.527115f, 0.527115f, 0.527115f, 1.f));

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button|Padding")
  float ButtonNormalPadding = 6.f;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button|Padding")
  float ButtonPressedPadding = 6.f;

  /** Default font size for ui.Text when size is not set explicitly (0 = use this row). */
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Text")
  float DefaultTextFontSize = 14.f;

  /** Slate typeface slot (e.g. Regular, Bold). */
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Text")
  FName TextTypefaceName = FName(TEXT("Regular"));

  /** Box / nine-slice texture for ui.Border panel background (e.g. EviUiPanel). */
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Border")
  TObjectPtr<UTexture2D> BorderPanelTexture;
};
