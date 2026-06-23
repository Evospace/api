#pragma once

#include "CoreMinimal.h"
#include "MapGeneratorSettings.generated.h"

// Common, generator-agnostic map settings. Held by UBiomeWorldGenerator and read
// in Generate(); legacy generators construct it with hardcoded values, the
// configurable generator uses the defaults below.
USTRUCT(BlueprintType)
struct FMapGeneratorSettings {
  GENERATED_BODY()

  // ON  -> terrain height = per-leaf-biome height maps (global reference field +
  //        each biome's authored detail, mask-blended across boundaries).
  // OFF -> legacy: terrain height = the global reference field only
  //        (EvalGlobalBiomeBaseHeight / EvalRiverShapedHeight), no per-biome detail.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Map")
  bool bUseBiomeHeightMaps = true;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Map")
  bool bGenerateRivers = true;
};
