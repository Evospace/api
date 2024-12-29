#pragma once
#include "Evospace/Vector.h"
#include "ThirdParty/luabridge/LuaBridge.h"
#include "MapStructure.generated.h"

class UStaticStructure;

USTRUCT(BlueprintType)
struct FMapStructure {
  GENERATED_BODY()

  UPROPERTY(VisibleAnywhere)
  FVector2i mOffset;

  UPROPERTY(VisibleAnywhere)
  UStaticStructure *mStruct;

  bool SerializeJson(TSharedPtr<FJsonObject> json);
  bool DeserializeJson(TSharedPtr<FJsonObject> json);

  static void lua_reg(lua_State *L) {
    luabridge::getGlobalNamespace(L)
      .beginClass<FMapStructure>("MapStructure") //class: MapStructure
      .addStaticFunction("new", [] { return FMapStructure(); })
      //direct:
      //--- Create new instance of MapStructure
      //--- @return MapStructure
      //function MapStructure.new() end
      .addProperty("offset", &FMapStructure::mOffset) //field: Vec2i
      .addProperty("structure", &FMapStructure::mStruct) //field: StaticStructure
      .endClass();
  }
};