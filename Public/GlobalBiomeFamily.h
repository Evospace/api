#pragma once
#include "MiscGeneratorClasses.h"
#include "GlobalBiomeFamily.generated.h"

struct FNoiseArray;

UCLASS()
class UGlobalBiomeFamily : public UBiomeFamily {
  GENERATED_BODY()
  PROTOTYPE_CODEGEN(GlobalBiomeFamily, Biome)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UGlobalBiomeFamily, UBiomeFamily>("GlobalBiomeFamily")
      .endClass();
  }
  virtual UClass *GetSuperProto() const override {
    return UBiome::StaticClass();
  }

  public:
  UGlobalBiomeFamily();

  virtual IndexType GetBiome(const Vec2i &pos) const override;

  virtual float GetHeight(const FVector2D &pos) const override;
  virtual float GetGrad(const FVector2D &pos) const override;
  virtual void GenerateGlobal(FColumnMaps &data, const FVector &pos, float Scale, int32 height_width, bool no_biom) const;

  void GetColumnMapsCell(const FNoiseArray &set_w, const FNoiseArray &set_h, const FNoiseArray &set_h2, const FNoiseArray &set_t, int i, float &out_h, float &out_b) const;

  float GetSphereHeight(FVector2D start_point) const;

  FColumnMaps mColumnMaps;
  std::unique_ptr<FastNoiseSIMD> mNoisePlanetSphere;
  std::unique_ptr<FastNoiseSIMD> mNoisePlanetSphereSmall;
  std::unique_ptr<FastNoiseSIMD> mNoisePlanetWetness;
  std::unique_ptr<FastNoiseSIMD> mNoisePlanetTemperature;

  virtual void SetSeed(int32 seed) override;
};