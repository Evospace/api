#pragma once
#include "Evospace/Blocks/Accessors/BaseInventorySideAccessor.h"
#include "SolidAccessor.generated.h"

UCLASS()
class EVOSPACE_API USolidAccessor : public UBaseInventoryAccessor {
  GENERATED_BODY()
  using Self = USolidAccessor;
  EVO_CODEGEN_ACCESSOR(SolidAccessor)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UBaseInventoryAccessor>("SolidAccessor") //@class SolidAccessor : BaseInventoryAccessor
      .endClass();
  }

public:
  USolidAccessor *GetOutsideAccessorCached();
  const USolidAccessor *GetOutsideAccessorCached() const;

protected:
  virtual UBaseInventoryAccessor *GetAutoOutsideAccessor() override;
};