// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Public/ModWidgets/ModWidget.h"
#include "ThirdParty/luabridge/LuaBridge.h"

#include "ModBorder.generated.h"

class UBorder;

/**
 * Border with a single mod-widget slot (padding configurable from Lua).
 */
UCLASS()
class UModBorder : public UModWidget {
  GENERATED_BODY()

  public:
  virtual void NativePreConstruct() override;

  UModWidget *GetModChild() const;
  void SetModChild(UModWidget *Child);
  void SetUniformPadding(float Uniform);

  static UModBorder *LuaNew();

  static void LuaRegister(lua_State *L) {
    using namespace luabridge;
    // clang-format off
    getGlobalNamespace(L)
      .deriveClass<UModBorder, UModWidget>("ModBorder") //@class ModBorder : ModWidget
      // direct:
      //--- Create a border with empty content
      //--- @return ModBorder
      // function ModBorder.new() end
      .addStaticFunction("new", &UModBorder::LuaNew)
      .addProperty("content", &UModBorder::GetModChild, &UModBorder::SetModChild) //@field ModWidget child widget
      // direct:
      //--- Uniform padding on all sides
      //--- @param uniform number Padding in slate units
      // function ModBorder:set_padding(uniform) end
      .addFunction("set_padding", &UModBorder::SetUniformPadding)
      .endClass();
    // clang-format on
  }

  private:
  void EnsureBorder();

  UPROPERTY()
  TObjectPtr<UBorder> BorderWidget;
};
