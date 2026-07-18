#pragma once
#include "GlobalBiomeFamily.h"
#include "GlobalBiomeFamilyConfigurable.generated.h"

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

  void ApplyLayoutScaleSettings(float BiomeSizeSpreadPercent, float ContinentScaleSpreadPercent);

  void SyncRegionalNoiseFrequencies(float biomeSizeMultiplier);

  void ApplyHeightSettings(
    bool InGenerateRivers,
    float PrimaryHeightMultiplier,
    float SecondaryHeightMultiplier,
    float InOceanFoldLevel,
    float InSeaRegionMultiplier,
    float InMountainRegionMultiplier);

  void EnsureHeightGatedBiomeIndicesCached();

  PROTOTYPE_CODEGEN(GlobalBiomeFamilyConfigurable, GlobalBiomeFamily)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UGlobalBiomeFamilyConfigurable, UGlobalBiomeFamily>("GlobalBiomeFamilyConfigurable")
      .endClass();
  }
  virtual UClass *GetSuperProto() const override { return UBiome::StaticClass(); }

  private:
  void CacheHeightGatedBiomeIndices();
  int32 PickHeightGatedBiomeIndex(float height, float cellularRaw, float mountainRegionRaw) const;

  std::unique_ptr<FastNoiseSIMD> river_noise;
  std::unique_ptr<FastNoiseSIMD> sea_region_noise;
  std::unique_ptr<FastNoiseSIMD> mountain_region_noise;

  float AuthoredBiomeFrequency = 0.003f;
  float AuthoredContinentFrequency = 0.0005f;

  static constexpr float SeaRegionBiomeFrequencyScale = 0.85f;
  static constexpr float MountainRegionBiomeFrequencyScale = 0.75f;

  bool bGenerateRivers = false;
  float PrimaryNoiseHeightMultiplier = 4.f;
  float SecondaryNoiseHeightMultiplier = 0.f;
  float OceanFoldLevel = -2.5f;
  float SeaRegionMultiplier = 1.f;
  float MountainRegionMultiplier = 1.f;

  int32 SnowBiomeIndex = INDEX_NONE;
  int32 MountainsBiomeIndex = INDEX_NONE;
  TArray<int32> CellularLandBiomeIndices;
};
