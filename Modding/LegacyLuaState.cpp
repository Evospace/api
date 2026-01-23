// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#include "LegacyLuaState.h"

#include "Public/BaseInventoryAccessor.h"

namespace evo {

static luabridge::LuaRef GetLegacyTable(lua_State *L) {
  luabridge::LuaRef legacy = luabridge::getGlobal(L, "Legacy");
  if (!legacy.isTable()) {
    legacy = luabridge::newTable(L);
    luabridge::setGlobal(L, legacy, "Legacy");
  }
  return legacy;
}

void LegacyLuaState::SetCurrent(AActor *actor) {
  luabridge::LuaRef legacy = GetLegacyTable(L);
  legacy["this"] = actor;
}

void LegacyLuaState::SetCurrent(UBlockLogic *actor) {
  luabridge::LuaRef legacy = GetLegacyTable(L);
  legacy["this"] = actor;
}

void LegacyLuaState::Init(::IRegistrar *registrar) {
  LuaState::Init(registrar);
  luabridge::LuaRef legacy = GetLegacyTable(L);
  legacy["this"] = nullptr;
}
} // namespace evo