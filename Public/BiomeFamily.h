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
      .endClass();
  }
  virtual UClass *GetSuperProto() const override { return UBiome::StaticClass(); }

  public:
  UBiomeFamily();

  // Only baked
  virtual FLayering GetLayering(const Vec2i &pos) const override;

  virtual float GetHeight(const FVector2D &pos) const override;

  // Only baked
  virtual const UStaticProp *GetSurfaceAttach(FRandomStream &rnd, const Vec2i &pos) const override;

  // Only baked
  virtual IndexType GetBiome(const Vec2i &pos) const override;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  virtual void SetSeed(int32 seed) override;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  TArray<UBiome *> mSubBiomes;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  float mSubFrequency = 0.016f;

  protected:
  std::unique_ptr<FastNoiseSIMD> mBiomeNoise;
  std::unique_ptr<FastNoiseSIMD> mWarpNoise;
  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  float WarpFrequency = 0.05f;
  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  int32 WarpOctaves = 4;
  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  float WarpAmplitude = 6.0f;
};
