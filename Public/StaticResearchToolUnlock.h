#pragma once
#include "StaticResearch.h"
#include "StaticResearchToolUnlock.generated.h"

UCLASS(BlueprintType)
class UStaticResearchToolUnlock : public UStaticResearch {
  GENERATED_BODY()
  using Self = UStaticResearchToolUnlock;
  PROTOTYPE_CODEGEN(StaticResearchToolUnlock, StaticResearch)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UStaticResearch>(
        "StaticResearchToolUnlock") //@class StaticResearchToolUnlock : StaticResearch
      .endClass();
  }
  virtual UClass *GetSuperProto() const override { return UStaticResearch::StaticClass(); }

  public:
  virtual void ApplyToController(AMainPlayerController *apply_to, int32 level) override;
};
