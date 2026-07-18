#pragma once

#include "CoreMinimal.h"
#include "Public/BiomeLayering.h"
#include "Qr/CoordinameMinimal.h"
#include "Qr/Prototype.h"
#include "Evospace/Shared/Public/HeightGenerator.h"
#include "Public/PropLayer.h"
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

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  UHeightGenerator *height = nullptr;

  UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0"))
  float WaterMurkiness = 0.0f;

  UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ClampMin = "0.0"))
  float SurfaceRockDetail = 0.0f;

  virtual TArray<UStaticWeather *> GetAvailableWeather() const { return TArray<UStaticWeather *>(); }
};