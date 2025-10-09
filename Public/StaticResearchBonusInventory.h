#pragma once
#include "StaticResearch.h"
#include "StaticResearchBonusInventory.generated.h"

UCLASS(BlueprintType)
class UStaticResearchBonusInventory : public UStaticResearch {
  GENERATED_BODY()
  using Self = UStaticResearchBonusInventory;
  PROTOTYPE_CODEGEN(StaticResearchBonusInventory, StaticResearch)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UStaticResearch>(
        "StaticResearchBonusInventory") //@class StaticResearchBonusInventory : StaticResearch
      .endClass();
  }
  virtual UClass *GetSuperProto() const override { return UStaticResearch::StaticClass(); }

  public:
  virtual void ApplyToController(AMainPlayerController *apply_to, int32 level) override;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  UPROPERTY(VisibleAnywhere)
  int BonusSlots = 11;
};
