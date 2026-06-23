// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once

#include "Public/WorldGenerator.h"
#include "MiscGeneratorClasses.h"
#include "MapGeneratorSettings.h"
#include "CoreMinimal.h"
#include "FastNoiseSIMD.h"

#include <UObject/Object.h>

#include <memory>
#include <utility>
#include <vector>

#include "BiomeWorldGenerator.generated.h"

struct FNoiseArray;
class UStaticBlock;
class UBiome;
class UGlobalBiomeFamily;
class USurfaceDefinition;

UCLASS(BlueprintType)
class UBiomeWorldGenerator : public UWorldGenerator {
  GENERATED_BODY()

  public:
  UBiomeWorldGenerator();

  virtual void Generate(FColumnLoaderData &data, const Vec3i &pos, USurfaceDefinition *surfaceDefinition) const override;
  virtual void GenerateGlobal(FColumnMaps &data, const FVector &pos, USurfaceDefinition *surfaceDefinition, float Scale, int32 height_width, bool no_biom) const override;

  virtual void SetSeed(int32 seed) override;

  // The configurable generator's whole settings bundle (height-map toggle,
  // rivers, carve) is read/written as one struct, so a menu UI can edit it on the
  // shared generator instance and then trigger a map regenerate.
  UFUNCTION(BlueprintCallable)
  FMapGeneratorSettings GetMapSettings() const { return MapSettings; }

  UFUNCTION(BlueprintCallable)
  virtual void SetMapSettings(const FMapGeneratorSettings &InSettings) { MapSettings = InSettings; }

  protected:
  virtual void Initialize() override;

  void PlaceProp(FRandomStream &stream, FTallSectorData &data, const FQrVector2i &sbpos,
                 const TArray<float> &BaseHeightCache, const TArray<float> &SurfaceHeightCache,
                 const TArray<UBiome *> &LeafBiomeCache, int w, int d) const;

  virtual void LoadBiomeFamily() override {}

  std::vector<UStaticBlock *> ores_map;

  // Common map settings (e.g. whether Generate() uses per-leaf biome height
  // maps). Legacy subclasses set these in their constructors; the configurable
  // generator keeps the defaults.
  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  FMapGeneratorSettings MapSettings;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  UGlobalBiomeFamily *mGlobalBiome;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  UStaticBlock *UnderworldBlock;

  std::unique_ptr<FastNoiseSIMD> ore_vein, ore_cell;
  // Canyon noises stored on the generator instance
  std::unique_ptr<FastNoiseSIMD> canyon_mask2d; // Simplex fractal mask in XY
  std::unique_ptr<FastNoiseSIMD> canyon_cell3d; // Cellular distance field in 3D

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

/**
 * Configurable generator: a copy of UWorldGeneratorRivers that drives its
 * behavior from MapSettings (biome height maps ON by default), as opposed to the
 * legacy generators above which hardcode them OFF.
 */
UCLASS(BlueprintType)
class UWorldGeneratorConfigurable : public UBiomeWorldGenerator {
  GENERATED_BODY()

  public:
  UWorldGeneratorConfigurable();
  virtual void LoadBiomeFamily() override;
  virtual void SetMapSettings(const FMapGeneratorSettings &InSettings) override;
};
