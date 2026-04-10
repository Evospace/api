// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Public/ModWidgets/ModWidget.h"
#include "ThirdParty/luabridge/LuaBridge.h"

#include "ModVerticalBox.generated.h"

class UVerticalBox;

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

  /** @callstyle ModVerticalBox { child1, child2, ... } or ModVerticalBox() */
  static int LuaBuildFromTable(lua_State *L);

  static void LuaRegister(lua_State *L) {
    using namespace luabridge;
    // clang-format off
    getGlobalNamespace(L)
      .deriveClass<UModVerticalBox, UModWidget>("ModVerticalBox") //@class ModVerticalBox : ModWidget
      // direct:
      //--- Create an empty vertical box
      //--- @return ModVerticalBox
      // function ModVerticalBox.new() end
      .addStaticFunction("new", &UModVerticalBox::LuaNew)
      // direct:
      //--- Append a child widget (must not already be parented)
      //--- @param child ModWidget
      // function ModVerticalBox:add(child) end
      .addFunction("add", &UModVerticalBox::AddModChild)
      .endClass();
    // clang-format on
  }

  private:
  void EnsureContainer();

  UPROPERTY()
  TObjectPtr<UVerticalBox> Container;
};
