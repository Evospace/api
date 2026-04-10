// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Public/ModWidgets/ModWidget.h"
#include "ThirdParty/luabridge/LuaBridge.h"

#include <string_view>

#include "ModTextBlock.generated.h"

class UTextBlock;

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

  static void LuaRegister(lua_State *L) {
    using namespace luabridge;
    // clang-format off
    getGlobalNamespace(L)
      .deriveClass<UModTextBlock, UModWidget>("ModTextBlock") //@class ModTextBlock : ModWidget
      // direct:
      //--- Create a text label with initial UTF-8 string
      //--- @param text string
      //--- @return ModTextBlock
      // function ModTextBlock.new(text) end
      .addStaticFunction("new", &UModTextBlock::LuaNew)
      // direct:
      //--- Set displayed text (UTF-8)
      //--- @param text string
      // function ModTextBlock:set_text(text) end
      .addFunction("set_text", &UModTextBlock::SetTextUtf8)
      .endClass();
    // clang-format on
  }

  private:
  void EnsureWidgetTree();

  UPROPERTY()
  TObjectPtr<UTextBlock> TextWidget;
};
