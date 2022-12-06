#pragma once
#include "CoreMinimal.h"
#include "legacy_lua_state.h"
#include "Evospace/CoordinateSystem.h"
#include "lua_state.h"

#include <string>

class UJsonObjectLibrary;
namespace evo {
/**
 * @brief Legacy lua state for BlockLogic lua script part executing
 */
class ModLoadingLuaState : public LegacyLuaState {
  public:
    ModLoadingLuaState();

    static void RegisterObject(UObject *val);

    static ModLoadingLuaState &Get();

  private:
    UJsonObjectLibrary *mLibrary = nullptr;
};
} // namespace evo