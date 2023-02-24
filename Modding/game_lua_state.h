// Copyright (c) 2017 - 2023, Samsonov Andrey. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "legacy_lua_state.h"

class ULuaBlock;
class UPrototype;
class UJsonObjectLibrary;
class URecipeDictionary;

namespace evo {
/**
 * @brief World tick lua state for BlockLogic lua script part executing
 */
class GameLuaState : public LuaState {
  public:
    GameLuaState();

  private:
    template <typename T> void registerCall(lua_State *L) {
        T::lua_reg(L);
        T::lua_reg_internal(L);
    }
};
} // namespace evo