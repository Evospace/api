#pragma once
#include "ConductorBlockLogic.h"
#include "FluidContainerBlockLogic.generated.h"

UCLASS()
class UFluidContainerBlockLogic : public UConductorBlockLogic {
  using Self = UFluidContainerBlockLogic;
  EVO_CODEGEN_INSTANCE(FluidContainerBlockLogic)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UConductorBlockLogic>("FluidContainerBlockLogic") //@class FluidContainerBlockLogic : ConductorBlockLogic
      .endClass();
  }
  GENERATED_BODY()
  public:
  UFluidContainerBlockLogic();

  virtual void BlockBeginPlay() override;

  virtual int32 GetChannel() const override;

  protected:
  // Inventory removed: network uses virtual storage at subnetwork level
};