#pragma once
#include "StaticResearch.h"
#include "StaticResearchBonusInventory.generated.h"

UCLASS(BlueprintType)
class EVOSPACE_API UStaticResearchBonusInventory : public UStaticResearch {
  GENERATED_BODY()
  using Self = UStaticResearchBonusInventory;
  EVO_OWNED(StaticResearchBonusInventory, StaticResearch)
  EVO_CODEGEN_DB(StaticResearchBonusInventory, StaticResearch)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UStaticResearch>("StaticResearchBonusInventory") //@class StaticResearchBonusInventory : StaticResearch
      .endClass();
  }
public:
  virtual void ApplyToController(AMainPlayerController *apply_to, int32 level) override;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  UPROPERTY(VisibleAnywhere)
  int BonusSlots = 11;
};
