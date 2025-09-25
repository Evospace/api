// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once
#include "StaticObject.h"
#include "Qr/CoordinameMinimal.h"

#include "StaticProp.generated.h"

class USectorProxy;
class ASector;
class UStaticMesh;
class AActor;

UCLASS()
class UStaticProp : public UStaticObject {
  using Self = UStaticProp;
  GENERATED_BODY()
  PROTOTYPE_CODEGEN(StaticProp, StaticProp)
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
      .addProperty("mesh", &Self::Mesh) //@field StaticMesh
      .addProperty("no_collision", &Self::NoCollision) //@field boolean
      .addProperty("is_emitting", &Self::IsEmitting) //@field boolean
      .addProperty("high_detail_shadow", &Self::HighDetailShadow) //@field boolean
      .addProperty("on_spawn", &Self::onSpawn) //@field function
      .addProperty("hits_to_break", &Self::HitsToBreak) //@field integer Number of hits to break
      .addProperty("break_effect", &Self::BreakEffect) //@field Class
      .addProperty("damage_effect", &Self::DamageEffect) //@field Class
      .addProperty("actor_class", &Self::ActorClass) //@field Class
      .endClass();
  }
  virtual UClass *GetSuperProto() const override {
    return StaticClass();
  }

  public:
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  void OnDestroy(const Vec3i &pos) const;
  void OnDamage(const Vec3i &pos) const;
  void OnSpawn(const Vec3i &pos) const;

  virtual bool Create(USectorProxy *sector, const FTransform &transform, const FVector3i &bpos) const;

  virtual void Release() override;

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
  int32 HitsToBreak = 1;

  UPROPERTY(EditAnywhere)
  UClass *BreakEffect = nullptr;

  UPROPERTY(EditAnywhere)
  UClass *DamageEffect = nullptr;

  UPROPERTY(VisibleAnywhere)
  TArray<UMaterialInterface *> Materials;

  UPROPERTY(VisibleAnywhere)
  UStaticMesh *Mesh;

  UPROPERTY(VisibleAnywhere)
  bool NoCollision = false;

  // If true, this prop can cast high-detail shadows, which can be globally toggled
  UPROPERTY(EditAnywhere)
  bool HighDetailShadow = false;

  // If set, this prop can be spawned as an actor decoration instead of instanced mesh
  UPROPERTY(EditAnywhere)
  UClass *ActorClass;

  std::optional<luabridge::LuaRef> onSpawn;
};
