#pragma once
#include "GlobalBiomeFamily.h"
#include "GlobalBiomeFamily3.generated.h"

UCLASS()
class UGlobalBiomeFamily3 : public UGlobalBiomeFamily {
  GENERATED_BODY()

  public:
  UGlobalBiomeFamily3();

  virtual IndexType GetBiome(const Vec2i &pos) const override;
  virtual void GenerateGlobal(FColumnMaps &data, const FVector &pos, float Scale, int32 height_width, bool no_biom) const override;
  virtual float GetHeight(const FVector2D &pos) const override;
  void GetColumnMapsCell(class UTextureReader *reader, const FNoiseArray &set_r, const FNoiseArray &set_w, const FNoiseArray &set_h, const FNoiseArray &set_h2, const FNoiseArray &set_t, int i, float &out_h, float &out_b) const;
  virtual void SetSeed(int32 seed) override;
  std::unique_ptr<FastNoiseSIMD> river_noise;

  public:
  PROTOTYPE_CODEGEN(GlobalBiomeFamily3, GlobalBiomeFamily)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UGlobalBiomeFamily3, UGlobalBiomeFamily>("GlobalBiomeFamily3")
      .endClass();
  }
  virtual UClass *GetSuperProto() const override {
    return UBiome::StaticClass();
  }
};