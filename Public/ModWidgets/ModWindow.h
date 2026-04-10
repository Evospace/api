// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Public/ModWidgets/ModWidget.h"
#include "ThirdParty/luabridge/LuaBridge.h"

#include <string_view>

#include "ModWindow.generated.h"

class UOverlay;

/**
 * Simple window with one content slot; show/close via viewport.
 * Title passed to Lua new() is not rendered (mods may still pass it for logging/API stability).
 */
UCLASS()
class UModWindow : public UModWidget {
  GENERATED_BODY()

  public:
  virtual void NativePreConstruct() override;

  UModWidget *GetContent() const;
  void SetContent(UModWidget *Content);
  void Show(int32 ZOrder = 0);
  void Close();

  /** Viewport pixel offsets from the anchor point; anchors 0..1 on each axis. */
  void SetWindowCanvasPlacement(float InAnchorX, float InAnchorY, float InOffsetX, float InOffsetY);

  static UModWindow *LuaNew(std::string_view TitleUtf8);

  /** @callstyle ModWindow "title" or ModWindow { title = "...", anchor_x = "left"|"center"|"right"?, anchor_y = "top"|"center"|"bottom"?, offset_x = n?, offset_y = n?, ModWidget at [1]? } */
  static int LuaBuildFromTable(lua_State *L);

  static void LuaRegister(lua_State *L) {
    using namespace luabridge;
    // clang-format off
    getGlobalNamespace(L)
      .deriveClass<UModWindow, UModWidget>("ModWindow") //@class ModWindow : ModWidget
      // direct:
      //--- Create a window (title is for API/logging only)
      //--- @param title string UTF-8 title
      //--- @return ModWindow
      // function ModWindow.new(title) end
      .addStaticFunction("new", &UModWindow::LuaNew)
      .addProperty("content", &UModWindow::GetContent, &UModWindow::SetContent) //@field ModWidget Single child (replaces previous when set)
      // direct:
      //--- Add to viewport at Z order 0
      // function ModWindow:show() end
      .addFunction("show", +[](UModWindow *Self) { Self->Show(0); })
      // direct:
      //--- Remove from parent (hide)
      // function ModWindow:close() end
      .addFunction("close", &UModWindow::Close)
      .endClass();
    // clang-format on
  }

  private:
  void EnsureWidgetTree();
  void ApplyContentOverlayCanvasLayout();

  float WindowAnchorX = 0.f;
  float WindowAnchorY = 0.f;
  float WindowOffsetX = 0.f;
  float WindowOffsetY = 0.f;

  UPROPERTY()
  TObjectPtr<UOverlay> ContentOverlay;
};
