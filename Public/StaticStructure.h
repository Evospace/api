#pragma once
#include "Prototype.h"
#include "Evospace/Misc/AssetOwner.h"
#include "StaticStructure.generated.h"
UCLASS(BlueprintType)
/**
 * Map generation script object with size and bytecode
 */
class EVOSPACE_API UStaticStructure : public UPrototype {
  GENERATED_BODY()
  EVO_CODEGEN_DB(StaticStructure, StaticStructure)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UStaticStructure, UInstance>("StaticStructure") //@class StaticStructure : Instance
      .addProperty("generate", &UStaticStructure::Generate) //@field function
      .addProperty("size", &UStaticStructure::mSize) //@field Vec2i
      .endClass();
  }
  virtual UClass *GetSuperProto() const override {
    return StaticClass();
  }

  public:
  std::optional<luabridge::LuaRef> Generate;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  FVector2i mSize;

  virtual void Release() override;
};