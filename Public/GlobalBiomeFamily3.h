#pragma once
#include "GlobalBiomeFamily.h"
#include "GlobalBiomeFamily3.generated.h"

class USurfaceDefinition;

UCLASS()
class UGlobalBiomeFamily3 : public UGlobalBiomeFamily {
  GENERATED_BODY()

  public:
  UGlobalBiomeFamily3();

  virtual IndexType GetBiome(const Vec2i &pos) const override;
  virtual void GenerateGlobal(FColumnMaps &data, const FVector &pos, USurfaceDefinition *surfaceDefinition, float Scale, int32 height_width, bool no_biom) const override;
  virtual float GetHeight(const FVector2D &pos) const override;
  virtual void SetSeed(int32 seed) override;
  std::unique_ptr<FastNoiseSIMD> river_noise;

  public:
  PROTOTYPE_CODEGEN(GlobalBiomeFamily3, GlobalBiomeFamily)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UGlobalBiomeFamily3, UGlobalBiomeFamily>("GlobalBiomeFamily3")
      .endClass();
  }
  virtual UClass *GetSuperProto() const override { return UBiome::StaticClass(); }
};