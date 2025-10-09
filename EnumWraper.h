#pragma once
#include "Evospace/Shared/ThirdParty/lua/lua.h"

/// Use as base for stack specializations to enable enum conversion
template <typename T>
struct EnumWrapper {
  static typename std::enable_if<std::is_enum<T>::value, void>::type push(lua_State *L, T value) {
    lua_pushnumber(L, static_cast<std::size_t>(value));
  }

  static typename std::enable_if<std::is_enum<T>::value, T>::type get(lua_State *L, int index) {
    return static_cast<T>(lua_tointeger(L, index));
  }
};