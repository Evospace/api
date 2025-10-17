#pragma once

#include "CoreMinimal.h"
#include "CarveSettings.generated.h"

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