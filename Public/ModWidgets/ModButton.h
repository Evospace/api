// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Public/ModWidgets/ModWidget.h"
#include "ThirdParty/luabridge/LuaBridge.h"

#include <optional>
#include <string_view>

#include "Components/Button.h"

#include "ModButton.generated.h"

class UTextBlock;

/**
 * Simple text button that invokes a Lua function on click.
 */
UCLASS()
class UModButton : public UModWidget {
  GENERATED_BODY()

  public:
  virtual void NativePreConstruct() override;
  virtual void BeginDestroy() override;

  void SetLabelText(const FText &NewLabel);

  UFUNCTION()
  void HandleClicked();

  std::optional<luabridge::LuaRef> OnClick;

  static UModButton *LuaNew(std::string_view LabelUtf8, const luabridge::LuaRef &OnClickRef);

  static void LuaRegister(lua_State *L) {
    using namespace luabridge;
    // clang-format off
    getGlobalNamespace(L)
      .deriveClass<UModButton, UModWidget>("ModButton") //@class ModButton : ModWidget
      // direct:
      //--- Create a label button; OnClick must be a Lua function
      //--- @param label string UTF-8 label text
      //--- @param on_click function Invoked on click
      //--- @return ModButton
      // function ModButton.new(label, on_click) end
      .addStaticFunction("new", &UModButton::LuaNew)
      .endClass();
    // clang-format on
  }

  private:
  void EnsureWidgetTree();

  UPROPERTY()
  TObjectPtr<UTextBlock> LabelTextBlock;

  UPROPERTY()
  TObjectPtr<UButton> ButtonWidget;
};
