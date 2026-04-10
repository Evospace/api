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

  void SetContent(UModWidget *Content);
  void Show(int32 ZOrder = 0);
  void Close();

  static UModWindow *LuaNew(std::string_view TitleUtf8);

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
      // direct:
      //--- Set single child content (replaces previous)
      //--- @param content ModWidget
      // function ModWindow:set_content(content) end
      .addFunction("set_content", &UModWindow::SetContent)
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

  UPROPERTY()
  TObjectPtr<UOverlay> ContentOverlay;
};
