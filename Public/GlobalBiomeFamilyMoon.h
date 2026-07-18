#pragma once
#include "GlobalBiomeFamily.h"
#include "GlobalBiomeFamilyMoon.generated.h"

UCLASS()
class UGlobalBiomeFamilyMoon : public UGlobalBiomeFamily {
  GENERATED_BODY()

  public:
  virtual float GetHeight(const FVector2D &pos) const override;
  virtual void GenerateGlobal(FColumnMaps &data, const FVector &pos, USurfaceDefinition *surfaceDefinition, float Scale, int32 height_width, bool no_biom) const override;

  PROTOTYPE_CODEGEN(GlobalBiomeFamilyMoon, GlobalBiomeFamily)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UGlobalBiomeFamilyMoon, UGlobalBiomeFamily>("GlobalBiomeFamilyMoon")
      .endClass();
  }
  virtual UClass *GetSuperProto() const override { return UBiome::StaticClass(); }

  private:
  static constexpr float kReferenceHeight = 0.f;
};
