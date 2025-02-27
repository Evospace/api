// Copyright (c) 2017 - 2024, Samsonov Andrey. All Rights Reserved.

#pragma once

#include "../CoordinateSystem.h"
#include "../NoiseArray.h"
#include "../Vector.h"
#include "Public/WorldGenerator.h"
#include "MiscGeneratorClasses.h"
#include "CoreMinimal.h"
#include "FastNoiseSIMD.h"

#include <UObject/Object.h>

#include <memory>
#include <utility>
#include <vector>

#include "BiomeWorldGenerator.generated.h"
class UStaticBlock;
class UGlobalBiomeFamily;

UCLASS(BlueprintType)
/**
 *  Legacy biome based world generator
 */
class UBiomeWorldGenerator : public UWorldGenerator {
  GENERATED_BODY()

  public:
  UBiomeWorldGenerator();

  virtual void Generate(FColumnSectorsData &data, const Vec3i &pos) const override;
  virtual void GenerateGlobal(FColumnMaps &data, const FVector &pos, float Scale = 1.f, int32 height_width = gFlatSectorSize.X, bool no_biom = false) const override;

  virtual void SetSeed(int32 seed) override;

  protected:
  virtual void Initialize() override;

  void PlaceProp(FRandomStream &stream, FTallSectorData &data, const FVector2i &sbpos, const Vec2i & orePos, TArray<float> &HeightCache, int w, int d) const;

  virtual void LoadBiomeFamily() override;

  std::vector<UStaticBlock *> ores_map;

  UPROPERTY()
  UGlobalBiomeFamily *mGlobalBiome;

  UPROPERTY()
  UStaticBlock *UnderworldBlock;

  std::unique_ptr<FastNoiseSIMD> ore_vein, ore_cell, caves, caves2;

  UPROPERTY()
  UStaticPropList *OreList;

  public:
  EVO_OWNED(BiomeWorldGenerator, WorldGenerator)
  EVO_CODEGEN_DB(BiomeWorldGenerator, WorldGenerator)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UBiomeWorldGenerator, UWorldGenerator>("WorldGeneratorBiome")
      .endClass();
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

UCLASS()
class UGlobalBiomeFamily : public UBiomeFamily {
  GENERATED_BODY()

  public:
  UGlobalBiomeFamily();

  virtual IndexType GetBiome(const Vec2i &pos) const override;

  virtual float GetHeight(const FVector2D &pos) const override;
  virtual float GetGrad(const FVector2D &pos) const override;
  virtual void GenerateGlobal(FColumnMaps &data, const FVector &pos, float Scale = 1.f, int32 height_width = gFlatSectorSize.X, bool no_biom = false) const;

  void GetColumnMapsCell(const FNoiseArray &set_w, const FNoiseArray &set_h, const FNoiseArray &set_h2, const FNoiseArray &set_t, int i, float &out_h, float &out_b) const;

  float GetSphereHeight(FVector2D start_point) const;

  FColumnMaps mColumnMaps;
  std::unique_ptr<FastNoiseSIMD> mNoisePlanetSphere;
  std::unique_ptr<FastNoiseSIMD> mNoisePlanetSphereSmall;
  std::unique_ptr<FastNoiseSIMD> mNoisePlanetWetness;
  std::unique_ptr<FastNoiseSIMD> mNoisePlanetTemperature;

  virtual void SetSeed(int32 seed) override;

  public:
  EVO_OWNED(GlobalBiomeFamily, Biome)
  EVO_CODEGEN_DB(GlobalBiomeFamily, BiomeFamily)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UGlobalBiomeFamily, UBiomeFamily>("GlobalBiomeFamily")
      .endClass();
  }
};

UCLASS()
class UGlobalBiomeFamily2 : public UGlobalBiomeFamily {
  GENERATED_BODY()

  public:
  UGlobalBiomeFamily2();

  virtual IndexType GetBiome(const Vec2i &pos) const override;
  virtual void GenerateGlobal(FColumnMaps &data, const FVector &pos, float Scale = 1.f, int32 height_width = gFlatSectorSize.X, bool no_biom = false) const;
  virtual float GetHeight(const FVector2D &pos) const override;
  void GetColumnMapsCell(const FNoiseArray &set_r, const FNoiseArray &set_w, const FNoiseArray &set_h, const FNoiseArray &set_h2, const FNoiseArray &set_t, int i, float &out_h, float &out_b) const;
  virtual void SetSeed(int32 seed) override;
  std::unique_ptr<FastNoiseSIMD> river_noise;

  public:
  EVO_OWNED(GlobalBiomeFamily2, Biome)
  EVO_CODEGEN_DB(GlobalBiomeFamily2, GlobalBiomeFamily)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UGlobalBiomeFamily2, UGlobalBiomeFamily>("GlobalBiomeFamily2")
      .endClass();
  }
};

UCLASS()
class UGlobalBiomeFamily3 : public UGlobalBiomeFamily {
  GENERATED_BODY()

  public:
  UGlobalBiomeFamily3();

  virtual IndexType GetBiome(const Vec2i &pos) const override;
  virtual void GenerateGlobal(FColumnMaps &data, const FVector &pos, float Scale = 1.f, int32 height_width = gFlatSectorSize.X, bool no_biom = false) const;
  virtual float GetHeight(const FVector2D &pos) const override;
  void GetColumnMapsCell(const FNoiseArray &set_r, const FNoiseArray &set_w, const FNoiseArray &set_h, const FNoiseArray &set_h2, const FNoiseArray &set_t, int i, float &out_h, float &out_b) const;
  virtual void SetSeed(int32 seed) override;
  std::unique_ptr<FastNoiseSIMD> river_noise;

  public:
  EVO_OWNED(GlobalBiomeFamily3, Biome)
  EVO_CODEGEN_DB(GlobalBiomeFamily3, GlobalBiomeFamily)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UGlobalBiomeFamily3, UGlobalBiomeFamily>("GlobalBiomeFamily3")
      .endClass();
  }
};

UENUM(BlueprintType)
enum class ENaturalBiome : uint8 {
  Tundra = 0, // (84, 234, 247)
  Taiga = 1, // (5, 102, 33)
  Steppe = 2, // (249, 218, 7)
  Rainforest = 3, // (7, 249, 162)
  Grassland = 4, // (155, 224, 35)
  Desert = 5, // (250, 147, 23)
  Forest = 6, // (46, 177, 83)
  TropicalForest = 7, // (8, 250, 54)
  Sea = 8, // (35, 48, 224)
  Volcano = 9, // (255, 0, 0)
};