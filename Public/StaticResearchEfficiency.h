#pragma once
#include "StaticResearch.h"
#include "StaticResearchEfficiency.generated.h"

UCLASS(BlueprintType)
class EVOSPACE_API UStaticResearchEfficiency : public UStaticResearch {
  GENERATED_BODY()
  using Self = UStaticResearchEfficiency;
  PROTOTYPE_CODEGEN(StaticResearchEfficiency, StaticResearch)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UStaticResearch>("StaticResearchEfficiency") //@class StaticResearchEfficiency : StaticResearch
      .endClass();
  }
  virtual UClass *GetSuperProto() const override {
    return UStaticResearch::StaticClass();
  }

  public:
  virtual void ApplyToController(AMainPlayerController *apply_to, int32 level) override;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  FName mRecipeDictionary;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  FName mRecipe;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  float mBonusValue;
};
