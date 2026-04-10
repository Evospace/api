// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Public/ModWidgets/ModWidget.h"
#include "ThirdParty/luabridge/LuaBridge.h"

#include "ModHorizontalBox.generated.h"

class UHorizontalBox;

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

  /** @callstyle ModHorizontalBox { child1, child2, ... } or ModHorizontalBox() */
  static int LuaBuildFromTable(lua_State *L);

  static void LuaRegister(lua_State *L) {
    using namespace luabridge;
    // clang-format off
    getGlobalNamespace(L)
      .deriveClass<UModHorizontalBox, UModWidget>("ModHorizontalBox") //@class ModHorizontalBox : ModWidget
      // direct:
      //--- Create an empty horizontal box
      //--- @return ModHorizontalBox
      // function ModHorizontalBox.new() end
      .addStaticFunction("new", &UModHorizontalBox::LuaNew)
      // direct:
      //--- Append a child widget (must not already be parented)
      //--- @param child ModWidget
      // function ModHorizontalBox:add(child) end
      .addFunction("add", &UModHorizontalBox::AddModChild)
      .endClass();
    // clang-format on
  }

  private:
  void EnsureContainer();

  UPROPERTY()
  TObjectPtr<UHorizontalBox> Container;
};
