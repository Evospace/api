// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Public/ModWidgets/ModWidget.h"
#include "ThirdParty/luabridge/LuaBridge.h"

#include <string>
#include <string_view>

#include "ModTextBlock.generated.h"

class UTextBlock;

/**
 * Text label for mod UI.
 */
UCLASS()
class UModTextBlock : public UModWidget {
  GENERATED_BODY()

  public:
  virtual void NativePreConstruct() override;

  std::string GetTextUtf8() const;
  void SetTextUtf8(std::string_view TextUtf8);
  void SetFontSize(float NewSize);

  static UModTextBlock *LuaNew(std::string_view InitialUtf8);

  /** @callstyle ModTextBlock "text" or ModTextBlock { text = "...", font_size = n? } */
  static int LuaBuildFromTable(lua_State *L);

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
      .addProperty("text", &UModTextBlock::GetTextUtf8, &UModTextBlock::SetTextUtf8) //@field string Displayed UTF-8 text
      // direct:
      //--- Set font size in slate units
      //--- @param size number
      // function ModTextBlock:set_font_size(size) end
      .addFunction("set_font_size", &UModTextBlock::SetFontSize)
      .endClass();
    // clang-format on
  }

  private:
  void EnsureWidgetTree();

  UPROPERTY()
  TObjectPtr<UTextBlock> TextWidget;

  UPROPERTY()
  float FontSize = 14.f;
};
