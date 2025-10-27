#pragma once
#include "StaticObject.h"
#include "StaticCover.generated.h"

UCLASS(BlueprintType)
class UStaticCover : public UStaticObject {
  GENERATED_BODY()
  PROTOTYPE_CODEGEN(StaticCover, StaticCover)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UStaticCover, UStaticObject>("StaticCover") //@class StaticCover : StaticObject
      .endClass();
  }
  virtual UClass *GetSuperProto() const override { return StaticClass(); }

  public:
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  UPROPERTY(VisibleAnywhere)
  TArray<UMaterialInterface *> Materials;

  UPROPERTY(VisibleAnywhere)
  UStaticMesh *Mesh;

  UPROPERTY(VisibleAnywhere)
  uint8 NumCustomData = 0;

  UPROPERTY(VisibleAnywhere)
  bool NoCollision = false;
};