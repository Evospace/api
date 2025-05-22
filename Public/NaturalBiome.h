#pragma once

#include "NaturalBiome.generated.h"

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