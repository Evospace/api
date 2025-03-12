// Copyright (c) 2017 - 2024, Samsonov Andrey. All Rights Reserved.
#include "EvospaceLuaState.h"
#include "Evospace/Blocks/CommonBlockTypes/ResourceComponent.h"
#include "LuaRegistration/ActorComponents.h"

#include <tuple>

namespace evo {

template <typename T>
void RegisterClassAndSuper(UClass *Class, TSet<UClass *> &Registered, lua_State *L, IRegistrar *registrar) {
  if (!Class || Registered.Contains(Class)) {
    return;
  }

  // Register superclass first
  auto super = Class->GetSuperClass();
  if (super && super != Class && super->IsChildOf(T::StaticClass())) {
    RegisterClassAndSuper<T>(Class->GetSuperClass(), Registered, L, registrar);
  }

  if (const auto cdo = Class->GetDefaultObject<T>()) {
    if (!Registered.Contains(cdo->lua_reg_type())) {
      cdo->register_owner(registrar);
      cdo->lua_reg(L);
      cdo->lua_reg_internal(L);
      Registered.Add(Class);
    }
  }
}

void ModLoadingLuaState::Init(IRegistrar *registrar) {
  LuaState::Init(registrar);

  TSet<UClass *> registered = {};
  for (TObjectIterator<UClass> it; it; ++it) {
    if (it->IsChildOf(UPrototype::StaticClass()) && !registered.Contains(*it)) {
      RegisterClassAndSuper<UPrototype>(*it, registered, L, registrar);
    } else if (it->IsChildOf(UInstance::StaticClass()) && !registered.Contains(*it)) {
      RegisterClassAndSuper<UInstance>(*it, registered, L, registrar);
    }
  }
}
} // namespace evo