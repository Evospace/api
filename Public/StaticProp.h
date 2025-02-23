// Copyright (c) 2017 - 2024, Samsonov Andrey. All Rights Reserved.

#pragma once
#include "Evospace/CoordinameMinimal.h"
#include "Public/StaticAttach.h"

#include "StaticProp.generated.h"

class ASector;
class UStaticMesh;

UCLASS(Abstract)
class EVOSPACE_API UStaticProp : public UStaticObject {
  using Self = UStaticProp;
  GENERATED_BODY()
  EVO_OWNER(StaticProp);
  EVO_CODEGEN_DB(StaticProp, StaticProp)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UStaticObject>("StaticProp") //@class StaticProp : StaticObject
      .addProperty("project_to_terrain_power", &Self::ProjectToTerrainPower) //@field number
      .addProperty("additive_elevation", &Self::AdditiveElevation) //@field number
      .addProperty("cull_begin", &Self::CullBegin) //@field number
      .addProperty("cull_end", &Self::CullEnd) //@field number
      .addProperty("maximum_height", &Self::MaximumHeight) //@field number
      .addProperty("minimum_height", &Self::MinimumHeight) //@field number
      .addProperty("floating", &Self::Floating) //@field boolean
      .addProperty("is_big", &Self::IsBig) //@field boolean
      .addProperty("mesh", &Self::Mesh) //@field StaticItem
      .addProperty("no_collision", &Self::NoCollision) //@field boolean
      .endClass();
  }

  public:
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  void OnDestroy(const Vec3i &pos) const;

  virtual bool Create(ASector *sector, const FTransform &transform, const FVector3i &bpos, bool ignored = false) const;

  UPROPERTY(EditAnywhere)
  float ScaleMin = .6;

  UPROPERTY(EditAnywhere)
  float ScaleMax = 1.4;

  UPROPERTY(EditAnywhere)
  float ProjectToTerrainPower = 1.0f;

  UPROPERTY(EditAnywhere)
  float AdditiveElevation = 0;

  UPROPERTY(EditAnywhere)
  float CullBegin = 0;

  UPROPERTY(EditAnywhere)
  float CullEnd = 0;

  UPROPERTY(EditAnywhere)
  float MaximumHeight = 100000;

  UPROPERTY(EditAnywhere)
  float MinimumHeight = -100000;

  UPROPERTY(EditAnywhere)
  bool Floating = false;

  UPROPERTY(EditAnywhere)
  bool IsBig = false;

  UPROPERTY(VisibleAnywhere)
  TArray<UMaterialInterface *> Materials;

  UPROPERTY(VisibleAnywhere)
  UStaticMesh *Mesh;

  UPROPERTY(VisibleAnywhere)
  bool NoCollision = false;
};
