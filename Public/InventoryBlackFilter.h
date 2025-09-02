#pragma once
#include "InventoryFilter.h"
#include "InventoryBlackFilter.generated.h"
UCLASS(BlueprintType)
class UInventoryBlackFilter : public UInventoryFilter {
  GENERATED_BODY()
  EVO_CODEGEN_INSTANCE(InventoryBlackFilter)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UInventoryBlackFilter, UInstance>("InventoryBlackFilter") //@class InventoryBlackFilter : InventoryFilter
      .endClass();
  }

  public:
  virtual bool Check(const FItemData &data) override;
};