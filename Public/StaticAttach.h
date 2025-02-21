
// Copyright (c) 2017 - 2024, Samsonov Andrey. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Public/StaticObject.h"

#include "StaticAttach.generated.h"
class ASector;

UCLASS(Abstract)
class EVOSPACE_API UStaticAttach : public UStaticObject {
  using Self = UStaticAttach;
  GENERATED_BODY()
  EVO_OWNER(StaticAttach);
  EVO_CODEGEN_DB(StaticAttach, StaticAttach);
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UStaticObject>("StaticAttach") //@class StaticAttach : StaticObject
      .addProperty("mesh", &Self::Mesh) //@field StaticItem
      .addProperty("no_collision", &Self::NoCollision) //@field boolean
      .addProperty("on_destroy", &Self::OnDestroy) //@field fun(pos: Vec3, attach: StaticAttach) comment example
      .endClass();
  }

  public:
  virtual bool Create(ASector *sector, const FTransform &transform, const FVector3i &bpos, bool ignored = false) const {
    return false;
  }

  virtual void Release() override;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  void CallDestroyScript(const FVector&pos) const;

  UPROPERTY(VisibleAnywhere)
  TArray<UMaterialInterface *> Materials;

  UPROPERTY(VisibleAnywhere)
  UStaticMesh *Mesh;

  UPROPERTY(VisibleAnywhere)
  bool NoCollision = false;
  
  std::optional<luabridge::LuaRef> OnDestroy;
};