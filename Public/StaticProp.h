// Copyright (c) 2017 - 2024, Samsonov Andrey. All Rights Reserved.

#pragma once
#include "Public/StaticAttach.h"

#include "StaticProp.generated.h"

class ASector;
class UStaticMesh;

UCLASS(Abstract)
class EVOSPACE_API UStaticProp : public UStaticAttach {
  using Self = UStaticProp;
  GENERATED_BODY()
  EVO_OWNED(StaticProp, StaticAttach);
  EVO_CODEGEN_DB(StaticProp, StaticAttach)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
    .deriveClass<Self, UStaticAttach>("StaticProp") //class: StaticProp, parent: StaticAttach
    .addProperty("lock_xy", &Self::LockXY) //field: boolean
    .addProperty("lock_all", &Self::LockAll) //field: boolean
    .addProperty("project_to_terrain_power", &Self::ProjectToTerrainPower) //field: number
    .addProperty("additive_elevation", &Self::AdditiveElevation) //field: number
    .addProperty("cull_begin", &Self::CullBegin) //field: number
    .addProperty("cull_end", &Self::CullEnd) //field: number
    .addProperty("maximum_height", &Self::MaximumHeight) //field: number
    .addProperty("minimum_height", &Self::MinimumHeight) //field: number
    .addProperty("floating", &Self::Floating) //field: boolean
    .addProperty("is_big", &Self::IsBig) //field: boolean
    .endClass();
  }

  public:
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  virtual bool Create(ASector *sector, const FTransform &transform, const FVector3i &bpos, bool ignored = false) const override;

  UPROPERTY(EditAnywhere)
  FVector ScaleMin = FVector{ 0.6, 0.6, 0.6 };

  UPROPERTY(EditAnywhere)
  FVector ScaleMax = FVector{ 1.4, 1.4, 1.4 };

  UPROPERTY(EditAnywhere)
  bool LockXY = true;

  UPROPERTY(EditAnywhere)
  bool LockAll = true;

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
};
