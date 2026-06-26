#pragma once
#include "CoreMinimal.h"
#include "Biome.h"
#include "FastNoiseSIMD.h"
#include "Qr/CommonConverter.h"
#include "BiomeFamily.generated.h"

UCLASS()
class UBiomeFamily : public UBiome {
  using Self = UBiomeFamily;
  GENERATED_BODY()
  PROTOTYPE_CODEGEN(BiomeFamily, Biome)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UBiomeFamily, UBiome>("BiomeFamily") //@class BiomeFamily : Biome
      .addProperty("sub_biomes", QR_ARRAY_GET_SET(mSubBiomes)) //@property Biome[]
      .addProperty("sub_frequency", &UBiomeFamily::mSubFrequency) //@property float
      .addProperty("perturb_amp", &UBiomeFamily::PerturbAmp) //@property float
      .addProperty("perturb_frequency", &UBiomeFamily::PerturbFrequency) //@property float
      .addProperty("perturb_octaves", &UBiomeFamily::PerturbOctaves) //@property int
      .endClass();
  }
  virtual UClass *GetSuperProto() const override { return UBiome::StaticClass(); }

  public:
  UBiomeFamily();

  // Only baked
  virtual FLayering GetLayering(const Vec2i &pos) const override;

  virtual void FillPropCandidates(TArray<FPropCandidate> &out, int32 seed, const Vec2i &sectorCorner, int32 sectorW,
                                  int32 sectorH) const override;

  // Only baked
  virtual IndexType GetBiome(const Vec2i &pos) const override;

  // Batched sub-biome selection over a [w x h] region anchored at world
  // (originX, originY): out[i] in [0, mSubBiomes.Num()-1]. Uses the same
  // domain-warped cellular field as GetBiome.
  void FillBiomeMap(int32 *out, int32 originX, int32 originY, int32 w, int32 h) const;

  // Raw cellular noise values (CellValue return type, [-1..1]) with domain warp
  // applied from Perturb* settings. Used by GetBiome, FillBiomeMap, and global
  // biome GenerateGlobal paths so point and bulk sampling stay consistent.
  void FillBiomeNoiseSet(float *out, float originX, float originY, int32 w, int32 h, float scale = 1.f) const;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  virtual void SetSeed(int32 seed) override;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  TArray<UBiome *> mSubBiomes;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  float mSubFrequency = 0.016f;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  float PerturbAmp = 28.f;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  float PerturbFrequency = 0.008f;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  int32 PerturbOctaves = 3;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  TArray<UStaticWeather *> AvailableWeather;

  /** Must have the same length as AvailableWeather when that list is non-empty; sums to positive weights for random pick. */
  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  TArray<int32> WeatherWeights;

  virtual TArray<UStaticWeather *> GetAvailableWeather() const override { return AvailableWeather; }

  protected:
  void ApplyBiomeNoiseSettings();
  void ComputeBiomeWarpOffset(float fx, float fy, float &outWx, float &outWy) const;

  std::unique_ptr<FastNoiseSIMD> mBiomeNoise;
  std::unique_ptr<FastNoiseSIMD> mWarpNoise;
};
