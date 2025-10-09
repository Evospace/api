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
USTRUCT(BlueprintType)
struct FCarveNoiseSettings {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Caves")
  bool bEnable = true;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Caves")
  float NoiseThreshold = 0.65f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Caves")
  float NoiseScale = 5.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Caves")
  float CarveStrength = 0.35f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Caves")
  float MaxDepthBlocks = 8.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Caves")
  float Frequency = 0.03f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Caves")
  int32 FractalOctaves = 3;

  // Canyon parameters
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Canyons")
  bool bCanyonEnable = true;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Canyons")
  float CanyonFrequency = 0.02f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Canyons")
  float CanyonPerturbFrequency = 2.f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Canyons")
  float CanyonPerturbAmp = 0.002f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Canyons")
  int32 CanyonFractalOctaves = 3;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Canyons")
  float CanyonThreshold = 0.7f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Canyons")
  float CanyonScale = 4.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Canyons")
  float CanyonCarveStrength = 4.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Canyons")
  float CanyonMaxDepthBlocks = 10.0f;

  // Canyon mask controls (2D simplex): disable canyons on ~80% of the map softly
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Canyons")
  float CanyonMaskFrequency = 0.016f;

  // Fraction of area where canyons are enabled (0..1). 0.2 => 80% disabled
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Canyons")
  float CanyonMaskCoverage = 0.35f;

  // Soft transition half-width in [0..1] space for mask gating
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Canyons")
  float CanyonMaskSoftness = 0.05f;
};

struct FNoiseArray;
class UStaticBlock;
class UGlobalBiomeFamily;
class USurfaceDefinition;

UCLASS(BlueprintType)
class UBiomeWorldGenerator : public UWorldGenerator {
  GENERATED_BODY()

  public:
  UBiomeWorldGenerator();

  virtual void Generate(FColumnLoaderData &data, const Vec3i &pos,
                        USurfaceDefinition *surfaceDefinition) const override;
  virtual void GenerateGlobal(FColumnMaps &data, const FVector &pos, float Scale, int32 height_width,
                              bool no_biom) const override;

  virtual void SetSeed(int32 seed) override;

  protected:
  virtual void Initialize() override;

  void PlaceProp(FRandomStream &stream, FTallSectorData &data, const FVector2i &sbpos, const Vec2i &orePos,
                 TArray<float> &HeightCache, int w, int d) const;

  virtual void LoadBiomeFamily() override {}

  std::vector<UStaticBlock *> ores_map;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  UGlobalBiomeFamily *mGlobalBiome;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  UStaticBlock *UnderworldBlock;

  std::unique_ptr<FastNoiseSIMD> ore_vein, ore_cell;
  // Canyon noises stored on the generator instance
  std::unique_ptr<FastNoiseSIMD> canyon_mask2d; // Simplex fractal mask in XY
  std::unique_ptr<FastNoiseSIMD> canyon_cell3d; // Cellular distance field in 3D

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Caves")
  FCarveNoiseSettings CarveSettings;

  public:
  PROTOTYPE_CODEGEN(BiomeWorldGenerator, WorldGenerator)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UBiomeWorldGenerator, UWorldGenerator>(
        "BiomeWorldGenerator") //@class BiomeWorldGenerator : WorldGenerator
      .addProperty("global_biome", &UBiomeWorldGenerator::mGlobalBiome) //@field GlobalBiomeFamily
      .endClass();
  }
  virtual UClass *GetSuperProto() const override { return UWorldGenerator::StaticClass(); }
};

/**
 *
 */
UCLASS(BlueprintType)
class UWorldGeneratorLegacy : public UBiomeWorldGenerator {
  GENERATED_BODY()

  public:
  UWorldGeneratorLegacy();
  virtual void LoadBiomeFamily() override;
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