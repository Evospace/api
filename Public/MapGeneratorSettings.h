#pragma once

#include "CoreMinimal.h"
#include "CarveSettings.h"
#include "MapGeneratorSettings.generated.h"

USTRUCT(BlueprintType)
struct FMapGeneratorSettings {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Map")
  bool bUseBiomeHeightMaps = true;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Map")
  bool bGenerateRivers = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Map", meta = (ClampMin = "0", ClampMax = "100"))
  float PrimaryNoiseHeightMultiplier = 4.f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Map", meta = (ClampMin = "0", ClampMax = "50"))
  float SecondaryNoiseHeightMultiplier = 0.f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Map", meta = (ClampMin = "-20", ClampMax = "0"))
  float OceanFoldLevel = -2.5f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Map", meta = (ClampMin = "0", ClampMax = "3"))
  float SeaRegionMultiplier = 1.f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Map", meta = (ClampMin = "0", ClampMax = "3"))
  float MountainRegionMultiplier = 1.f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Map", meta = (ClampMin = "-50", ClampMax = "100"))
  float BiomeSizeSpreadPercent = 0.f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Map", meta = (ClampMin = "-50", ClampMax = "100"))
  float ContinentScaleSpreadPercent = 0.f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Caves")
  FCarveNoiseSettings CarveSettings;
};
