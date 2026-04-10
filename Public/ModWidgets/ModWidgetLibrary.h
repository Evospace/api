// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "Public/ModWidgets/ModBorder.h"
#include "Public/ModWidgets/ModButton.h"
#include "Public/ModWidgets/ModHorizontalBox.h"
#include "Public/ModWidgets/ModTextBlock.h"
#include "Public/ModWidgets/ModVerticalBox.h"
#include "Public/ModWidgets/ModWidget.h"
#include "Public/ModWidgets/ModWindow.h"

struct lua_State;

/**
 * Registers mod widget Lua types (bindings live in widget headers for api.lua codegen).
 */
class UModWidgetLibrary {
  public:
  static void lua_reg(lua_State *L);
};
