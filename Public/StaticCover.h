#pragma once
#include "StaticObject.h"
#include "StaticCover.generated.h"

UCLASS()
class EVOSPACE_API UStaticCover : public UStaticObject {
  GENERATED_BODY()
  EVO_OWNER(StaticCover)
  EVO_CODEGEN_DB(StaticCover, StaticCover)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UStaticCover, UStaticObject>("StaticCover") //@class StaticCover : StaticObject
      .endClass();
  }

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