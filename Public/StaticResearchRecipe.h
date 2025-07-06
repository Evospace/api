#pragma once
#include "StaticResearch.h"
#include "StaticResearchRecipe.generated.h"

UCLASS(BlueprintType)
class EVOSPACE_API UStaticResearchRecipe : public UStaticResearch {
  GENERATED_BODY()
  using Self = UStaticResearchRecipe;
  PROTOTYPE_CODEGEN(StaticResearchRecipe, StaticResearch)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UStaticResearch>("StaticResearchRecipe") //@class StaticResearchRecipe : StaticResearch
      .endClass();
  }
  virtual UClass *GetSuperProto() const override {
    return UStaticResearch::StaticClass();
  }

  public:
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  TArray<class URecipe *> RecipeUnlocks;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  FVector2i LevelMinMax;

  virtual void ComputeSearchMetadata() const override;

  virtual void ApplyToController(AMainPlayerController *apply_to, int32 level) override;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  virtual FText GetDescription() const override;

  virtual UTexture2D *GetTexture() const override;
};
