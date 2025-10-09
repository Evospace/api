#pragma once
#include "Qr/Vector.h"
#include "ThirdParty/luabridge/LuaBridge.h"
#include "MapStructure.generated.h"

class UStaticStructure;

USTRUCT(BlueprintType)
struct FMapStructure {
  GENERATED_BODY()

  UPROPERTY(VisibleAnywhere)
  FVector2i Offset;

  UPROPERTY(VisibleAnywhere)
  UStaticStructure *Structure;

  bool SerializeJson(TSharedPtr<FJsonObject> json) const;
  bool DeserializeJson(TSharedPtr<FJsonObject> json);

  void Release();

  static void lua_reg(lua_State *L) {
    luabridge::getGlobalNamespace(L)
      .beginClass<FMapStructure>("MapStructure") //@class MapStructure
      // direct:
      //--- Create new instance of MapStructure
      //--- @return MapStructure
      // function MapStructure.new() end
      .addStaticFunction("new", [] { return FMapStructure(); })
      .addProperty("offset", &FMapStructure::Offset) //@field Vec2i
      .addProperty("structure", &FMapStructure::Structure) //@field StaticStructure
      .endClass();
  }
};