#pragma once

#include "NaturalBiome.generated.h"

UENUM(BlueprintType)
enum class ENaturalBiome : uint8 {
  Sea = 0, // (84, 234, 247)
  Taiga = 1, // (5, 102, 33)
  Steppe = 2, // (249, 218, 7)
  PineForest = 3, // (7, 249, 162)
  Grassland = 4, // (155, 224, 35)
  Desert = 5, // (250, 147, 23)
  Forest = 6, // (46, 177, 83)
  Swamp = 7, // (8, 250, 54)
  Volcano = 8, // (255, 0, 0)
  FertileForest = 9, // top-level fertile forest biome
  Wasteland = 10, // dry wasteland biome for concrete worlds
};