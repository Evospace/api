// Copyright (c) 2017 - 2024, Samsonov Andrey. All Rights Reserved.
#include "LegacyLuaState.h"

#include "Evospace/Blocks/Accessors/BaseInventorySideAccessor.h"
#include "Public/SelectCrafter.h"
#include "Evospace/Item/InventoryContainer.h"
#include "Public/ResourceAccessor.h"

using namespace luabridge;

namespace evo {

void LegacyLuaState::SetCurrent(AActor *actor) {
  getGlobalNamespace(L).beginNamespace("Legacy").addVariable("this", actor).endNamespace();
}

void LegacyLuaState::SetCurrent(UBlockLogic *actor) {
  getGlobalNamespace(L).beginNamespace("Legacy").addVariable("this", actor).endNamespace();
}

void LegacyLuaState::Init() {
  ModLoadingLuaState::Init();
  using namespace luabridge;

  getGlobalNamespace(L).beginNamespace("Legacy").addVariable("this", nullptr).endNamespace();
}
} // namespace evo