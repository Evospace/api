#pragma once
#include "Qr/Prototype.h"
#include "MapgenData.generated.h"

UCLASS()
class EVOSPACE_API UMapgenData : public UInstance {
  GENERATED_BODY()
  EVO_CODEGEN_INSTANCE(MapgenData)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UMapgenData, UInstance>("MapgenData") //@class MapgenData : Instance

      .endClass();
  }

  public:
  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  int32 WaterLevel;
};