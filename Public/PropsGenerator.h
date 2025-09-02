#pragma once

#include "CoreMinimal.h"
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
      .addProperty("proplist", &UPropsGenerator::proplist) //@property StaticPropList
      .endClass();
  }
  virtual UClass *GetSuperProto() const override {
    return StaticClass();
  }

  public:
  virtual const UStaticProp *GetSurfaceAttach(FRandomStream &rnd, const Vec2i &start_point) const;
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  virtual void SetSeed(int32 seed);

  UPROPERTY()
  UStaticPropList *proplist = nullptr;
};