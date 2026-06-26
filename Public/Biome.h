#pragma once

#include "CoreMinimal.h"
#include "Public/BiomeLayering.h"
#include "Qr/CoordinameMinimal.h"
#include "Qr/Prototype.h"
#include "Evospace/Shared/Public/HeightGenerator.h"
#include "PropLayer.h"
#include "Biome.generated.h"

class ULayeringGenerator;
class UStaticProp;
class UPropsGenerator;
class UStaticWeather;

UCLASS()
class UBiome : public UPrototype {
  GENERATED_BODY()
  PROTOTYPE_CODEGEN(Biome, Biome)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UBiome, UPrototype>("Biome") //@class Biome : Prototype
      .endClass();
  }
  virtual UClass *GetSuperProto() const override { return StaticClass(); }

  public:
  virtual FLayering GetLayering(const Vec2i &pos) const;
  // Per-biome height *detail* (no global base elevation). Default returns the
  // owned height generator's value, or 0 when none is authored — a biome
  // without a height generator legitimately contributes no detail (e.g. sea).
  virtual float GetHeight(const FVector2D &pos) const;
  virtual void FillPropCandidates(TArray<FPropCandidate> &out, int32 seed, const Vec2i &sectorCorner, int32 sectorW,
                                  int32 sectorH) const;
  virtual IndexType GetBiome(const Vec2i &pos) const;
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual void SetSeed(int32 seed);

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  ULayeringGenerator *layering = nullptr;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  UPropsGenerator *props = nullptr;

  // Owns this leaf biome's terrain height detail (JSON "Height"). May be null.
  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  UHeightGenerator *height = nullptr;

  /** Local water murkiness for this leaf biome (0 = clear, 1 = opaque). */
  UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0"))
  float WaterMurkiness = 0.0f;

  virtual TArray<UStaticWeather *> GetAvailableWeather() const { return TArray<UStaticWeather *>(); }
};