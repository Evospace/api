#pragma once
#include "ThirdParty/luabridge/LuaBridge.h"
#include "Public/StaticProp.h"
#include "Evospace/Misc/EvoConverter.h"
#include "PropListData.generated.h"

USTRUCT()
struct FPropListData {
  GENERATED_BODY()
  using Self = FPropListData;
  static void lua_reg(lua_State *L) {
    luabridge::getGlobalNamespace(L)
      //@comment Structure that stores several StaticProp records (prop variations) with shared spawn chance
      .beginClass<FPropListData>("PropListData") //@class PropListData : Struct
      //direct:
      //--- @return PropListData
      //function PropListData.new() end
      .addStaticFunction("new", []() { return FPropListData(); })
      .addProperty("chance", &FPropListData::Chance) //@field number Spawn probability 0.0–1.0
      .addProperty("props", EVO_ARRAY_GET_SET(Props)) //@field StaticProp[] List of prop assets (prop variations)
      .endClass();
  }

  public:
  UPROPERTY()
  TArray<const UStaticProp *> Props;

  UPROPERTY()
  float Chance = 1.0;
};