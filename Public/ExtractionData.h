#pragma once
#include "ThirdParty/luabridge/LuaBridge.h"
#include "ExtractionData.generated.h"

class UStaticProp;
class UStaticItem;

USTRUCT(BlueprintType)
struct FExtractionData {
  GENERATED_BODY()
  using Self = FExtractionData;
  static void lua_reg(lua_State *L) {
    luabridge::getGlobalNamespace(L)
      .beginClass<Self>("ExtractionData") //@class ExtractionData
      .addStaticFunction("new", []() { return FExtractionData(); })
      //direct:
      //--- Create new instance of ExtractionData
      //--- @return ExtractionData
      //function ExtractionData.new() end
      .addProperty("item", &Self::Item) //@field StaticItem
      .addProperty("speed", &Self::Speed) //@field integer
      .addProperty("initial_capacity", &Self::InitialCapacity) //@field integer
      .endClass();
  }

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int32 Speed = 100;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int64 InitialCapacity = 10000;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  const UStaticItem *Item = nullptr;
};