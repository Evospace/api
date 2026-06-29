#pragma once

#include "CoreMinimal.h"
#include "CarveSettings.h"
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
  bool bGenerateRivers = false;

  // Height contribution of the primary (planet-sphere) noise on
  // UWorldGeneratorConfigurable. Low values keep the global reference field flat.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Map", meta = (ClampMin = "0", ClampMax = "100"))
  float PrimaryNoiseHeightMultiplier = 4.f;

  // Height contribution of the secondary (small-sphere) noise on
  // UWorldGeneratorConfigurable. Usually 0 for the flat configurable reference field.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Map", meta = (ClampMin = "0", ClampMax = "50"))
  float SecondaryNoiseHeightMultiplier = 0.f;

  // Ocean fold level (in blocks, below the height-0 water surface). Terrain that
  // ends up deeper than this gets reflected upward, so the deepest seafloor
  // surfaces as islands and the open-water footprint shrinks WITHOUT raising the
  // map (spawn at 0,0 stays at height 0, water stays at 0). Closer to 0 => less
  // ocean (shallower seas, more islands); more negative => more ocean. At -20 the
  // fold barely triggers (~original ~50% ocean). Consumed only by the configurable
  // generator's height curve. Default targets roughly 10-15% ocean.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Map", meta = (ClampMin = "-20", ClampMax = "0"))
  float OceanFoldLevel = -2.5f;

  // Regional shallow seas at biome-family scale. 0 disables; higher = deeper local basins.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Map", meta = (ClampMin = "0", ClampMax = "3"))
  float SeaRegionMultiplier = 1.f;

  // Regional mountain clusters at biome-family scale. 0 disables; higher values
  // lower the regional-noise threshold (more/larger clusters) and raise peak uplift.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Map", meta = (ClampMin = "0", ClampMax = "3"))
  float MountainRegionMultiplier = 1.f;

  // Percent offset applied to authored biome cell size on UWorldGeneratorConfigurable
  // only. 0 = default; +100 doubles biome regions; -50 halves them.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Map", meta = (ClampMin = "-50", ClampMax = "100"))
  float BiomeSizeSpreadPercent = 0.f;

  // Percent offset applied to authored continent (planet-sphere) scale on
  // UWorldGeneratorConfigurable only. Same range semantics as BiomeSizeSpreadPercent.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Map", meta = (ClampMin = "-50", ClampMax = "100"))
  float ContinentScaleSpreadPercent = 0.f;

  // Caves/canyons carving. Lives here (not on USurfaceDefinition) so every
  // generator setting travels as one unit: the menu edits one struct and the
  // generator reads one struct. Consumed by UBiomeWorldGenerator::Generate
  // (the 3D column pass); it does not affect the 2D GenerateGlobal preview.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Caves")
  FCarveNoiseSettings CarveSettings;
};
