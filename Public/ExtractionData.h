#pragma once
#include "ThirdParty/luabridge/LuaBridge.h"
#include "ExtractionData.generated.h"

class UStaticItem;

USTRUCT(BlueprintType)
struct FExtractionData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float mSpeed;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UStaticItem *mItem;

  static void lua_reg(lua_State *L) {
    luabridge::getGlobalNamespace(L)
      .beginClass<FExtractionData>("ExtractionData") //class: ExtractionData
      .addStaticFunction("new", []() { return FExtractionData(); })
      //direct:
      //--- Create new instance of ExtractionData
      //--- @return ExtractionData
      //function ExtractionData.new() end
      .addProperty("item", &FExtractionData::mItem) //field: StaticItem
      .addProperty("speed", &FExtractionData::mSpeed) //field: number
      .endClass();
  }
};