#pragma once
#include "ConductorBlockLogic.h"
#include "DataConductorBlockLogic.generated.h"
UCLASS()
class EVOSPACE_API UDataConductorBlockLogic : public UConductorBlockLogic {
  GENERATED_BODY()
  using Self = UDataConductorBlockLogic;
  EVO_CODEGEN_INSTANCE(DataConductorBlockLogic)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UConductorBlockLogic>("DataConductorBlockLogic")
      .endClass();
  }

  public:
  UDataConductorBlockLogic();

  virtual TSubclassOf<UBlockWidget> GetWidgetClass() const override;

  virtual int32 GetChannel() const override { return 6000; }

  bool IsDataNetwork() const override;
};