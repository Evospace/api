#pragma once
#include "StaticAttach.h"
#include "StaticCover.generated.h"

UCLASS()
class EVOSPACE_API UStaticCover : public UStaticAttach {
  GENERATED_BODY()
  EVO_OWNER(StaticCover)
  EVO_CODEGEN_DB(StaticCover, StaticCover)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UStaticCover, UStaticAttach>("StaticAttach") //class: StaticCover, parent: StaticAttach
      .endClass();
  }

  public:
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  int32 Tier = INDEX_NONE;
};