#pragma once

#include "CoreMinimal.h"
#include "Public/BiomeLayering.h"
#include "Qr/CoordinameMinimal.h"
#include "Qr/Prototype.h"
#include "Evospace/Shared/Public/HeightGenerator.h"
#include "Biome.generated.h"

class ULayeringGenerator;
class UStaticProp;
class UPropsGenerator;

UCLASS()
class EVOSPACE_API UBiome : public UPrototype {
  GENERATED_BODY()
  PROTOTYPE_CODEGEN(Biome, Biome)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UBiome, UPrototype>("Biome") //@class Biome : Prototype
      .addProperty("height", &UBiome::height)
      .endClass();
  }
  virtual UClass *GetSuperProto() const override {
    return StaticClass();
  }

  public:
  virtual FLayering GetLayering(const Vec2i &pos) const;
  virtual float GetHeight(const FVector2D &pos) const;
  // Returns biome-local height adjusted relative to provided global height (with internal limiting)
  virtual float GetHeightRelative(const FVector2D &pos, float globalHeight) const;
  virtual const UStaticProp *GetSurfaceAttach(FRandomStream &rnd, const Vec2i &pos) const;
  virtual float GetWeight(const FVector2D &pos) const;
  virtual IndexType GetBiome(const Vec2i &pos) const;
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual void SetSeed(int32 seed);

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  ULayeringGenerator *layering = nullptr;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  UPropsGenerator *props = nullptr;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  UHeightGenerator *height = nullptr;
};