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
  virtual float GetHeight(const FVector2D &pos) const PURE_VIRTUAL(UBiome::GetHeight, return 0.f;) virtual const UStaticProp *GetSurfaceAttach(FRandomStream &rnd, const Vec2i &pos) const;
  virtual IndexType GetBiome(const Vec2i &pos) const;
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual void SetSeed(int32 seed);

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  ULayeringGenerator *layering = nullptr;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  UPropsGenerator *props = nullptr;
};