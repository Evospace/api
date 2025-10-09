#pragma once
#include "StaticResearch.h"
#include "StaticResearchDecorationUnlock.generated.h"

UCLASS(BlueprintType)
class UStaticResearchDecorationUnlock : public UStaticResearch {
  GENERATED_BODY()
  using Self = UStaticResearchDecorationUnlock;
  PROTOTYPE_CODEGEN(StaticResearchDecorationUnlock, StaticResearch)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UStaticResearch>(
        "StaticResearchDecorationUnlock") //@class StaticResearchDecorationUnlock : StaticResearch
      .endClass();
  }
  virtual UClass *GetSuperProto() const override { return UStaticResearch::StaticClass(); }

  public:
  virtual void ApplyToController(AMainPlayerController *apply_to, int32 level) override;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  virtual FText GetDescription() const override;

  virtual UTexture2D *GetTexture() const override;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UStaticItem *Decoration;
};