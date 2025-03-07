// Copyright (c) 2017 - 2024, Samsonov Andrey. All Rights Reserved.
#pragma once
#include "Prototype.h"
#include "Evospace/Common.h"
#include "Evospace/Misc/AssetOwner.h"
#include "Evospace/World/BlockCell.h"

#include "WorldGenerator.generated.h"

class UStaticItem;

UCLASS(Abstract, BlueprintType)
class EVOSPACE_API UWorldGenerator : public UPrototype {
  GENERATED_BODY()

  public:
  virtual void Generate(FColumnSectorsData &data, const Vec3i &pos) const;

  UFUNCTION(BlueprintCallable)
  virtual void GenerateGlobal(FColumnMaps &data, const FVector &pos, float Scale = 1.f, int32 height_width = 16, bool no_biome = false) const;

  UFUNCTION(BlueprintCallable)
  virtual void SetSeed(int32 seed);

  UFUNCTION(BlueprintCallable)
  virtual void Initialize();

  UFUNCTION(BlueprintCallable)
  virtual void LoadBiomeFamily();

  UFUNCTION(BlueprintCallable)
  FText GetLabel() const;

  UPROPERTY()
  FKeyTableObject mLabel;

  protected:
  int32 mSeed = 0;

  public:
  EVO_OWNER(WorldGenerator)
  EVO_CODEGEN_DB(WorldGenerator, WorldGenerator)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UWorldGenerator, UPrototype>("WorldGenerator")
      .endClass();
  }
};