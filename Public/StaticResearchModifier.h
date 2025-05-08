#pragma once
#include "StaticResearch.h"
#include "StaticResearchModifier.generated.h"

UCLASS(BlueprintType)
class EVOSPACE_API UStaticResearchModifier : public UStaticResearch {
  GENERATED_BODY()
  using Self = UStaticResearchModifier;
  EVO_OWNED(StaticResearchModifier, StaticResearch)
  EVO_CODEGEN_DB(StaticResearchModifier, StaticResearch)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UStaticResearch>("StaticResearchModifier") //@class StaticResearchModifier : StaticResearch
      .endClass();
  }
public:
  virtual void ApplyToController(AMainPlayerController *apply_to, int32 level) override;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  class UStaticModifier *mModifier = nullptr;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  float mBonusValue;
};
