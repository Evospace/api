// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Public/ModWidgets/ModWidget.h"
#include "ThirdParty/luabridge/LuaBridge.h"

#include <optional>

#include "Components/Button.h"

#include "ModButton.generated.h"

class UTexture2D;

/**
 * Clickable button with a single mod-widget content slot.
 */
UCLASS()
class UModButton : public UModWidget {
  GENERATED_BODY()

  public:
  virtual void NativePreConstruct() override;
  virtual void BeginDestroy() override;

  UModWidget *GetModChild() const;
  void SetModChild(UModWidget *Child);

  UFUNCTION()
  void HandleClicked();

  std::optional<luabridge::LuaRef> OnClick;

  static UModButton *LuaNew(const luabridge::LuaRef &OnClickRef);

  /** @callstyle ModButton(fn) or ModButton { on_click = fn, ModWidget child at [1] } */
  static int LuaBuildFromTable(lua_State *L);

  static void LuaRegister(lua_State *L) {
    using namespace luabridge;
    // clang-format off
    getGlobalNamespace(L)
      .deriveClass<UModButton, UModWidget>("ModButton") //@class ModButton : ModWidget
      // direct:
      //--- Create a button with content slot; OnClick must be a Lua function
      //--- @param on_click function Invoked on click
      //--- @return ModButton
      // function ModButton.new(on_click) end
      .addStaticFunction("new", &UModButton::LuaNew)
      .addProperty("content", &UModButton::GetModChild, &UModButton::SetModChild) //@field ModWidget child widget
      .endClass();
    // clang-format on
  }

  private:
  void EnsureWidgetTree();

  UPROPERTY()
  TObjectPtr<UButton> ButtonWidget;

  UPROPERTY()
  TObjectPtr<UTexture2D> DefaultButtonHighlightTexture;

  UPROPERTY()
  TObjectPtr<UTexture2D> DefaultButtonHighlightFillTexture;
};
