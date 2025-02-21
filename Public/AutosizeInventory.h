#pragma once
#include "Inventory.h"
#include "AutosizeInventory.generated.h"

UCLASS(BlueprintType)
class EVOSPACE_API UAutosizeInventory : public UInventory {
  GENERATED_BODY()
  EVO_CODEGEN_INSTANCE(AutosizeInventory)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UAutosizeInventory, UInventory>("AutosizeInventory") //@class AutosizeInventory : Inventory
      .endClass();
  }

  public:
  UAutosizeInventory();
};

UCLASS(BlueprintType)
class EVOSPACE_API UNonSerializedAutosizeInventory : public UAutosizeInventory {
  GENERATED_BODY()
};
