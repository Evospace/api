#pragma once
#include "GlobalBiomeFamily.h"
#include "GlobalBiomeFamily2.generated.h"

UCLASS()
class UGlobalBiomeFamily2 : public UGlobalBiomeFamily {
  GENERATED_BODY()

  public:
  UGlobalBiomeFamily2();

  virtual IndexType GetBiome(const Vec2i &pos) const override;
  virtual void GenerateGlobal(FColumnMaps &data, const FVector &pos, USurfaceDefinition *surfaceDefinition, float Scale, int32 height_width, bool no_biom) const override;
  virtual float GetHeight(const FVector2D &pos) const override;
  virtual void SetSeed(int32 seed) override;

  public:
  PROTOTYPE_CODEGEN(GlobalBiomeFamily2, GlobalBiomeFamily)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UGlobalBiomeFamily2, UGlobalBiomeFamily>("GlobalBiomeFamily2")
      .endClass();
  }
  virtual UClass *GetSuperProto() const override { return UBiome::StaticClass(); }

  private:
  std::unique_ptr<FastNoiseSIMD> river_noise;
};