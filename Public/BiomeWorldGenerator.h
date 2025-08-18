// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once

#include "Public/WorldGenerator.h"
#include "MiscGeneratorClasses.h"
#include "CoreMinimal.h"
#include "FastNoiseSIMD.h"

#include <UObject/Object.h>

#include <memory>
#include <utility>
#include <vector>

#include "BiomeWorldGenerator.generated.h"

// Removed sub-biome export; no lookup struct needed.

struct FNoiseArray;
class UStaticBlock;
class UGlobalBiomeFamily;

UCLASS(BlueprintType)
class UBiomeWorldGenerator : public UWorldGenerator {
  GENERATED_BODY()

  public:
  UBiomeWorldGenerator();

  virtual void Generate(FColumnLoaderData &data, const Vec3i &pos) const override;
  virtual void GenerateGlobal(FColumnMaps &data, const FVector &pos, float Scale, int32 height_width, bool no_biom) const override;

  virtual void SetSeed(int32 seed) override;

  protected:
  virtual void Initialize() override;

  void PlaceProp(FRandomStream &stream, FTallSectorData &data, const FVector2i &sbpos, const Vec2i &orePos, TArray<float> &HeightCache, int w, int d) const;

  virtual void LoadBiomeFamily() override;

  std::vector<UStaticBlock *> ores_map;

  UPROPERTY()
  UGlobalBiomeFamily *mGlobalBiome;

  UPROPERTY()
  UStaticBlock *UnderworldBlock;

  std::unique_ptr<FastNoiseSIMD> ore_vein, ore_cell;

  // Removed sub-biome export; no lookup API

  public:
  PROTOTYPE_CODEGEN(BiomeWorldGenerator, WorldGenerator)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UBiomeWorldGenerator, UWorldGenerator>("WorldGeneratorBiome")
      .endClass();
  }
  virtual UClass *GetSuperProto() const override {
    return UWorldGenerator::StaticClass();
  }
};

/**
 *
 */
UCLASS(BlueprintType)
class UWorldGeneratorRivers : public UBiomeWorldGenerator {
  GENERATED_BODY()

  public:
  UWorldGeneratorRivers();
  virtual void LoadBiomeFamily() override;
};

/**
 *
 */
UCLASS(BlueprintType)
class UWorldGeneratorPlains : public UBiomeWorldGenerator {
  GENERATED_BODY()

  public:
  UWorldGeneratorPlains();
  virtual void LoadBiomeFamily() override;
};