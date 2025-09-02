// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "Qr/Prototype.h"
#include "Evospace/World/BlockCell.h"
#include "Qr/Loc.h"

#include "WorldGenerator.generated.h"

class UStaticItem;

UCLASS(Abstract, BlueprintType)
class UWorldGenerator : public UPrototype {
  GENERATED_BODY()

  public:
  virtual void Generate(FColumnLoaderData &data, const Vec3i &pos) const;

  UFUNCTION(BlueprintCallable)
  virtual void GenerateGlobal(FColumnMaps &data, const FVector &pos, float Scale, int32 height_width, bool no_biome) const;

  UFUNCTION(BlueprintCallable)
  virtual void SetSeed(int32 seed);

  UFUNCTION(BlueprintCallable)
  virtual void Initialize();

  //TODO: remove
  UFUNCTION(BlueprintCallable)
  virtual void LoadBiomeFamily();

  UFUNCTION(BlueprintCallable)
  FText GetLabel() const;

  UPROPERTY()
  FLoc mLabel;

  protected:
  int32 mSeed = 0;

  public:
  PROTOTYPE_CODEGEN(WorldGenerator, WorldGenerator)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UWorldGenerator, UPrototype>("WorldGenerator") //@class WorldGenerator : Prototype
      .endClass();
  }
  virtual UClass *GetSuperProto() const override {
    return StaticClass();
  }
};