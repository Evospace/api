// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#include "LegacyLuaState.h"

#include "Evospace/Blocks/Accessors/BaseInventorySideAccessor.h"

namespace evo {

void LegacyLuaState::SetCurrent(AActor *actor) {
  luabridge::getGlobalNamespace(L).beginNamespace("Legacy").addVariable("this", actor).endNamespace();
}

void LegacyLuaState::SetCurrent(UBlockLogic *actor) {
  luabridge::getGlobalNamespace(L).beginNamespace("Legacy").addVariable("this", actor).endNamespace();
}

void LegacyLuaState::Init(IRegistrar *registrar) {
  LuaState::Init(registrar);
  luabridge::getGlobalNamespace(L).beginNamespace("Legacy").addVariable("this", nullptr).endNamespace();
}
} // namespace evo