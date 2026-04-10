// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ThirdParty/luabridge/LuaBridge.h"

#include <string>

#include "ModWidget.generated.h"

/**
 * Base class for mod-built UMG widgets exposed to Lua.
 */
UCLASS(Abstract)
class UModWidget : public UUserWidget {
  GENERATED_BODY()

  public:
  std::string ToString() const { return TCHAR_TO_UTF8(*("(ModWidget: " + GetName() + ")")); }

  /** Expects a Lua stack index pointing to a userdata bound as ModWidget; errors if wrong type. */
  static UModWidget *GetChildFromStack(lua_State *L, int Idx);

  static void LuaRegister(lua_State *L) {
    using namespace luabridge;
    // clang-format off
    getGlobalNamespace(L)
      .deriveClass<UModWidget, UObject>("ModWidget") //@class ModWidget : Object
      // direct:
      //--- Debug string for print/tostring
      //--- @return string
      // function ModWidget:__tostring() end
      .addFunction("__tostring", &UModWidget::ToString)
      .endClass();
    // clang-format on
  }
};
