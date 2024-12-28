// Copyright (c) 2017 - 2024, Samsonov Andrey. All Rights Reserved.
#include "LegacyLuaState.h"

#include "Evospace/Blocks/Accessors/BaseInventorySideAccessor.h"
#include "Public/SelectCrafter.h"
#include "Evospace/Item/InventoryContainer.h"
#include "Public/ResourceAccessor.h"

using namespace luabridge;

namespace evo {
int LegacyLuaState::Accessor_bind(lua_State *l) {
  auto side_acc = Stack<UBaseInventoryAccessor *>::get(l, 1);
  auto container = Stack<UInventoryContainer *>::get(l, 2);

  side_acc.value()->Bind(container.value());

  return 0;
}

int LegacyLuaState::Crafter_get_resource_component(lua_State *l) {
  const auto self = Stack<UBlockLogic *>::get(l, 1);

  if (const auto cra = Cast<USelectCrafter>(self.value())) {
    auto result = push(l, cra->EnergyInputInventory);
  }

  return 1;
}

int LegacyLuaState::Crafter_get_input_container(lua_State *l) {
  const auto self = Stack<UBlockLogic *>::get(l, 1);

  if (const auto cra = Cast<USelectCrafter>(self.value())) {
    auto result = push(l, cra->CrafterInputContainer);
  }

  return 1;
}

int LegacyLuaState::Crafter_get_output_container(lua_State *l) {
  auto self = Stack<UBlockLogic *>::get(l, 1);

  auto cra = Cast<USelectCrafter>(self.value());
  if (cra) {
    auto result = push(l, cra->CrafterOutputContainer);
  }

  return 1;
}

int LegacyLuaState::Accessor_set_side_pos(lua_State *l) {
  auto self = Stack<UAccessor *>::get(l, 1);
  auto side = Stack<Vec3i>::get(l, 2);
  auto pos = Stack<Vec3i>::get(l, 3);
  self.value()->SetSidePos(side.value(), pos.value());
  return 0;
}

int LegacyLuaState::BlockLogic_new_resource_accessor(lua_State *l) {
  auto self = Stack<UBlockLogic *>::get(l, 1);
  auto name = Stack<std::string>::get(l, 2);

  auto accessor = NewObject<UResourceAccessor>(self.value(), UResourceAccessor::StaticClass(), FName(UTF8_TO_TCHAR(name.value().data())));
  self.value()->RegisterAccessor(accessor);
  auto result = push(l, accessor);

  return 1;
}

int LegacyLuaState::BlockLogic_new_item_accessor(lua_State *l) {
  auto self = Stack<UBlockLogic *>::get(l, 1);
  auto type = Stack<UClass *>::get(l, 2);
  std::error_code ec;

  if (type.value() && type.value()->IsChildOf(UAccessor::StaticClass())) {
    auto accessor = NewObject<UBaseInventoryAccessor>(self.value(), type.value());
    self.value()->RegisterAccessor(accessor);
    auto result = push(l, accessor);
  } else {
    auto result = push(l, LuaNil());
  }

  return 1;
}

void LegacyLuaState::SetCurrent(AActor *actor) {
  getGlobalNamespace(L).beginNamespace("Legacy").addVariable("this", actor).endNamespace();
}

void LegacyLuaState::SetCurrent(UBlockLogic *actor) {
  getGlobalNamespace(L).beginNamespace("Legacy").addVariable("this", actor).endNamespace();
}

void LegacyLuaState::Init() {
  ModLoadingLuaState::Init();
  using namespace luabridge;

  getGlobalNamespace(L)
    .beginClass<UBlockLogic>("BlockLogic")
    .addFunction("CreateAccessor", &LegacyLuaState::BlockLogic_new_item_accessor)
    .addFunction("new_resource_accessor", &LegacyLuaState::BlockLogic_new_resource_accessor)
    .addFunction("get_resource_component", &LegacyLuaState::Crafter_get_resource_component)
    .addFunction("GetInputContainer", &LegacyLuaState::Crafter_get_input_container)
    .addFunction("GetOutputContainer", &LegacyLuaState::Crafter_get_output_container)
    .endClass();

  getGlobalNamespace(L)
    .deriveClass<USelectCrafter, UBlockLogic>("SelectCrafterBlockLogic")
    .endClass();

  getGlobalNamespace(L).addFunction(
    "LoadAsset", +[](std::string_view name) {
      return LoadObject<UObject>(nullptr, UTF8_TO_TCHAR(name.data()));
    });

  getGlobalNamespace(L)
    .beginClass<UAccessor>("Accessor")
    .addFunction("SetSidePos", &LegacyLuaState::Accessor_set_side_pos)
    .endClass();

  getGlobalNamespace(L)
    .deriveClass<UBaseInventoryAccessor, UAccessor>("InventoryAccessor")
    .addFunction("Bind", &LegacyLuaState::Accessor_bind)
    .endClass();

  getGlobalNamespace(L).beginNamespace("Legacy").addVariable("this", nullptr).endNamespace();
}
} // namespace evo