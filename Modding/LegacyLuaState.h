// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "EvospaceLuaState.h"
#include "Public/BlockLogic.h"
#include "LuaState.h"

namespace evo {
/**
 * @brief Legacy lua state for BlockLogic lua script part executing
 */
class LegacyLuaState : public ModLoadingLuaState {
  public:
  void SetCurrent(AActor *actor);
  void SetCurrent(UBlockLogic *actor);

  virtual void Init(IRegistrar *registrar) override;

  AActor *current_object;
};
} // namespace evo