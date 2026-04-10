// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Public/ModWidgets/ModWidget.h"
#include "ThirdParty/luabridge/LuaBridge.h"

#include "ModBorder.generated.h"

class UBorder;
class UTexture2D;

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
  float GetUniformPadding() const;
  void SetUniformPadding(float Uniform);

  static UModBorder *LuaNew();

  /** @callstyle ModBorder { padding = n?, ModWidget at [1]? } */
  static int LuaBuildFromTable(lua_State *L);

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
      .addProperty("padding", &UModBorder::GetUniformPadding, &UModBorder::SetUniformPadding) //@field number Uniform padding in slate units (all sides)
      .endClass();
    // clang-format on
  }

  private:
  void EnsureBorder();

  UPROPERTY()
  TObjectPtr<UBorder> BorderWidget;

  UPROPERTY()
  TObjectPtr<UTexture2D> DefaultBorderTexture;
};
