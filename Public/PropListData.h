#pragma once
#include "ThirdParty/luabridge/LuaBridge.h"
#include "Public/StaticProp.h"
#include "PropListData.generated.h"

USTRUCT()
struct FPropListData {
  GENERATED_BODY()

  static void lua_reg(lua_State *L) {
    luabridge::getGlobalNamespace(L)
      .beginClass<FPropListData>("PropListData") //@class PropListData : Struct
      .addProperty("chance", &FPropListData::Chance) //@field number
      .addProperty("props", [](const FPropListData *self) //@field StaticProp[]
                   {
                     std::vector<const UStaticProp *> arr;
                     for (auto p : self->Props) {
                       arr.push_back(p);
                     }
                     return arr;
                   })
      .endClass();
  }

  public:
  UPROPERTY()
  TArray<const UStaticProp *> Props;

  UPROPERTY()
  float Chance = 1.0;
};