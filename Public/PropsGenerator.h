#pragma once

#include "CoreMinimal.h"
#include "PropLayer.h"
#include "Qr/CoordinameMinimal.h"
#include "Qr/Prototype.h"
#include "PropsGenerator.generated.h"

class UStaticPropList;
class UStaticProp;

UCLASS()
class UPropsGenerator : public UPrototype {
  GENERATED_BODY()
  PROTOTYPE_CODEGEN(PropsGenerator, PropsGenerator)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UPropsGenerator, UPrototype>("PropsGenerator") //@class PropsGenerator : Prototype
      .endClass();
  }
  virtual UClass *GetSuperProto() const override { return StaticClass(); }

  public:
  void FillCandidates(TArray<FPropCandidate> &out, int32 seed, const Vec2i &sectorCorner, int32 sectorW,
                      int32 sectorH) const;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  virtual void SetSeed(int32 seed);

  TArray<FPropLayer> Layers;
};
