#pragma once
#include "GlobalBiomeFamily.h"
#include "GlobalBiomeFamilyConfigurable.generated.h"

// Deep copy of UGlobalBiomeFamily2 (the "rivers" global family), owned by
// UWorldGeneratorConfigurable so its height/biome pipeline can evolve
// independently of the legacy generators. Not a subclass of UGlobalBiomeFamily2
// on purpose — the implementation is duplicated, not delegated.
UCLASS()
class UGlobalBiomeFamilyConfigurable : public UGlobalBiomeFamily {
  GENERATED_BODY()

  public:
  UGlobalBiomeFamilyConfigurable();

  virtual IndexType GetBiome(const Vec2i &pos) const override;
  virtual void GenerateGlobal(FColumnMaps &data, const FVector &pos, USurfaceDefinition *surfaceDefinition, float Scale, int32 height_width, bool no_biom) const override;
  virtual float GetHeight(const FVector2D &pos) const override;
  virtual void SetSeed(int32 seed) override;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  // Adjusts cellular-biome and continent noise frequencies relative to authored
  // defaults. SpreadPercent in [-50, 100]: 0 = authored size, +100 = 2x larger.
  void ApplyLayoutScaleSettings(float BiomeSizeSpreadPercent, float ContinentScaleSpreadPercent);

  PROTOTYPE_CODEGEN(GlobalBiomeFamilyConfigurable, GlobalBiomeFamily)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UGlobalBiomeFamilyConfigurable, UGlobalBiomeFamily>("GlobalBiomeFamilyConfigurable")
      .endClass();
  }
  virtual UClass *GetSuperProto() const override { return UBiome::StaticClass(); }

  private:
  std::unique_ptr<FastNoiseSIMD> river_noise;

  float AuthoredBiomeFrequency = 0.003f;
  float AuthoredContinentFrequency = 0.0005f;
};
