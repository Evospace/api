// Copyright (c) 2017 - 2024, Samsonov Andrey. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "LuaState.h"

namespace evo {
/**
 * @brief Mod loading process lua state
 */
class ModLoadingLuaState : public LuaState {

  public:
  virtual void Init(IRegistrar *registrar) override;
};
} // namespace evo