// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "ThirdParty/luabridge/luabridge.h"

#include "ModWidget.generated.h"

class UBorder;
class UHorizontalBox;
class UOverlay;
class UTextBlock;
class UVerticalBox;

/**
 * Base class for mod-built UMG widgets exposed to Lua.
 */
UCLASS(Abstract)
class UModWidget : public UUserWidget {
  GENERATED_BODY()

  public:
  std::string ToString() const { return TCHAR_TO_UTF8(*("(ModWidget: " + GetName() + ")")); }
};

/**
 * Simple text button that invokes a Lua function on click.
 */
UCLASS()
class UModButton : public UModWidget {
  GENERATED_BODY()

  public:
  virtual void NativePreConstruct() override;
  virtual void BeginDestroy() override;

  void SetLabelText(const FText &NewLabel);

  UFUNCTION()
  void HandleClicked();

  std::optional<luabridge::LuaRef> OnClick;

  static UModButton *LuaNew(std::string_view LabelUtf8, const luabridge::LuaRef &OnClickRef);

  private:
  void EnsureWidgetTree();

  UPROPERTY()
  TObjectPtr<UTextBlock> LabelTextBlock;

  UPROPERTY()
  TObjectPtr<UButton> ButtonWidget;
};

/**
 * Horizontal row container for mod widgets.
 */
UCLASS()
class UModHorizontalBox : public UModWidget {
  GENERATED_BODY()

  public:
  virtual void NativePreConstruct() override;

  void AddModChild(UModWidget *Child);

  static UModHorizontalBox *LuaNew();

  private:
  void EnsureContainer();

  UPROPERTY()
  TObjectPtr<UHorizontalBox> Container;
};

/**
 * Border with a single mod-widget slot (padding configurable from Lua).
 */
UCLASS()
class UModBorder : public UModWidget {
  GENERATED_BODY()

  public:
  virtual void NativePreConstruct() override;

  void SetModChild(UModWidget *Child);
  void SetUniformPadding(float Uniform);

  static UModBorder *LuaNew();

  private:
  void EnsureBorder();

  UPROPERTY()
  TObjectPtr<UBorder> BorderWidget;
};

/**
 * Read-only text label for mod UI.
 */
UCLASS()
class UModTextBlock : public UModWidget {
  GENERATED_BODY()

  public:
  virtual void NativePreConstruct() override;

  void SetTextUtf8(std::string_view TextUtf8);

  static UModTextBlock *LuaNew(std::string_view InitialUtf8);

  private:
  void EnsureWidgetTree();

  UPROPERTY()
  TObjectPtr<UTextBlock> TextWidget;
};

/**
 * Vertical column container for mod widgets.
 */
UCLASS()
class UModVerticalBox : public UModWidget {
  GENERATED_BODY()

  public:
  virtual void NativePreConstruct() override;

  void AddModChild(UModWidget *Child);

  static UModVerticalBox *LuaNew();

  private:
  void EnsureContainer();

  UPROPERTY()
  TObjectPtr<UVerticalBox> Container;
};

/**
 * Simple window with a title and one content slot; show/close via viewport.
 */
UCLASS()
class UModWindow : public UModWidget {
  GENERATED_BODY()

  public:
  virtual void NativePreConstruct() override;

  void SetWindowTitle(const FText &Title);
  void SetContent(UModWidget *Content);
  void Show(int32 ZOrder = 0);
  void Close();

  static UModWindow *LuaNew(std::string_view TitleUtf8);

  private:
  void EnsureWidgetTree();

  UPROPERTY()
  TObjectPtr<UTextBlock> TitleTextBlock;

  UPROPERTY()
  TObjectPtr<UOverlay> ContentOverlay;
};

/**
 * Registers mod widget types for Lua (see cpp for full list).
 */
class UModWidgetLibrary {
  public:
  static void lua_reg(lua_State *L);
};
