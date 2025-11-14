#pragma once
#include "BiomeFamily.h"
#include "Evospace/World/BlockCell.h"
#include "GlobalBiomeFamily.generated.h"

struct FNoiseArray;
class USurfaceDefinition;

UCLASS()
class UGlobalBiomeFamily : public UBiomeFamily {
  GENERATED_BODY()
  PROTOTYPE_CODEGEN(GlobalBiomeFamily, Biome)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UGlobalBiomeFamily, UBiomeFamily>("GlobalBiomeFamily") //@class GlobalBiomeFamily : BiomeFamily
      .endClass();
  }
  virtual UClass *GetSuperProto() const override { return UBiome::StaticClass(); }

  public:
  UGlobalBiomeFamily();

  virtual IndexType GetBiome(const Vec2i &pos) const override;

  virtual float GetHeight(const FVector2D &pos) const override;
  virtual void GenerateGlobal(FColumnMaps &data, const FVector &pos, USurfaceDefinition *surfaceDefinition, float Scale, int32 height_width, bool no_biom) const;

  virtual void SetSeed(int32 seed) override;

  protected:
  float GetSphereHeight(FVector2D start_point) const;
  std::unique_ptr<FastNoiseSIMD> mNoisePlanetSphere;
  std::unique_ptr<FastNoiseSIMD> mNoisePlanetSphereSmall;
};