// Copyright (c) 2017 - 2024, Samsonov Andrey. All Rights Reserved.

#pragma once
#include "StaticObject.h"
#include "Evospace/CoordinameMinimal.h"

#include "StaticProp.generated.h"

class ASector;
class UStaticMesh;

UCLASS()
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
      .addProperty("is_emitting", &Self::IsEmitting) //@field boolean
      .addProperty("break_chance", &Self::BreakChance) //@field integer Break chance in percents
      .endClass();
  }

  public:
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  void OnDestroy(const Vec3i &pos) const;
  void OnDamage(const Vec3i &pos) const;
  void OnSpawn(const Vec3i &pos) const;

  virtual bool Create(ASector *sector, const FTransform &transform, const FVector3i &bpos, bool ignored = false) const;

  virtual bool ProtoValidCheck() override;

  UPROPERTY(EditAnywhere)
  bool IsEmitting = false;

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

  UPROPERTY(EditAnywhere)
  int32 BreakChance = 100;

  UPROPERTY(VisibleAnywhere)
  TArray<UMaterialInterface *> Materials;

  UPROPERTY(VisibleAnywhere)
  UStaticMesh *Mesh;

  UPROPERTY(VisibleAnywhere)
  bool NoCollision = false;
};
