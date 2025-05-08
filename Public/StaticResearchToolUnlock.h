#pragma once
#include "StaticResearch.h"
#include "StaticResearchToolUnlock.generated.h"

UCLASS(BlueprintType)
class EVOSPACE_API UStaticResearchToolUnlock : public UStaticResearch {
  GENERATED_BODY()
  using Self = UStaticResearchToolUnlock;
  EVO_OWNED(StaticResearchToolUnlock, StaticResearch)
  EVO_CODEGEN_DB(StaticResearchToolUnlock, StaticResearch)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UStaticResearch>("StaticResearchToolUnlock") //@class StaticResearchToolUnlock : StaticResearch
      .endClass();
  }
public:
  virtual void ApplyToController(AMainPlayerController *apply_to, int32 level) override;
};
