
// Copyright (c) 2017 - 2024, Samsonov Andrey. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Public/StaticObject.h"

#include "StaticAttach.generated.h"
class ASector;

UCLASS(Abstract)
class EVOSPACE_API UStaticAttach : public UStaticObject {
  GENERATED_BODY()
  EVO_OWNER(StaticAttach);
  EVO_CODEGEN_DB(StaticAttach, StaticAttach);
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UStaticAttach, UStaticObject>("StaticAttach") //class: StaticAttach, parent: StaticObject
      .endClass();
  }

  public:
  virtual bool Create(ASector *sector, const FTransform &transform, const FVector3i &bpos, bool ignored = false) const {
    return false;
  }

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  UPROPERTY(VisibleAnywhere)
  TArray<UMaterialInterface *> Materials;

  UPROPERTY(VisibleAnywhere)
  UStaticMesh *StaticMesh;

  UPROPERTY(VisibleAnywhere)
  bool NoCollision = false;
};