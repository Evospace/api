// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "ReverbParameters.generated.h"

USTRUCT(BlueprintType)
struct FReverbParameters {
  GENERATED_BODY()

  // Room size estimation (0-1, from average distance)
  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  float RoomSize = 0.5f;

  // Wet signal level (reverb amount, 0-1)
  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  float WetLevel = 0.5f;

  // Dry signal level (direct sound, 0-1)
  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  float DryLevel = 1.0f;

  // Pre-delay time in milliseconds (distance-based)
  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  float Delay = 10.0f;

  // Overall reverb gain (0-1)
  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  float Gain = 0.7f;

  // Frequency bandwidth (0-1, material absorption)
  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  float Bandwidth = 0.8f;

  // Diffusion amount (0-1, surface complexity)
  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  float Diffusion = 0.7f;

  // High-frequency dampening (0-1, material absorption)
  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  float Dampening = 0.5f;

  // Decay time in seconds (RT60)
  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  float Decay = 1.5f;

  // Echo density (0-1, reflections per second)
  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  float Density = 0.8f;

  // Average absorption coefficient from materials (0-1)
  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  float AverageAbsorption = 0.3f;

  // Average distance to surfaces
  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  float AverageDistance = 1000.0f;

  // Wind stereo balance: 0 = left ear, 1 = right ear
  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  float WindBalance = 0.5f;

  FReverbParameters() {}
};

// Material absorption coefficients for common surface types
struct FMaterialAcousticProperties {
  float AbsorptionCoefficient = 0.3f;  // 0=reflective, 1=absorptive
  float Diffusion = 0.5f;               // 0=smooth, 1=rough
  float Dampening = 0.5f;               // High-frequency absorption

  static FMaterialAcousticProperties GetPropertiesForSurface(const FName &MaterialName) {
    FMaterialAcousticProperties Props;

    // Get the string for comparison
    FString MatStr = MaterialName.ToString();

    // Evospace Physical Materials - exact name matching
    if (MatStr.Contains(TEXT("EvoStone"))) {
      // Stone - very reflective, hard surfaces
      Props.AbsorptionCoefficient = 0.02f; // Very reflective
      Props.Diffusion = 0.3f;
      Props.Dampening = 0.2f;
    }
    else if (MatStr.Contains(TEXT("EvoMetal"))) {
      // Metal - highly reflective
      Props.AbsorptionCoefficient = 0.05f; // Very reflective
      Props.Diffusion = 0.2f;
      Props.Dampening = 0.1f;
    }
    else if (MatStr.Contains(TEXT("EvoGlass"))) {
      // Glass - very reflective, smooth
      Props.AbsorptionCoefficient = 0.03f; // Very reflective
      Props.Diffusion = 0.1f;
      Props.Dampening = 0.15f;
    }
    else if (MatStr.Contains(TEXT("EvoWater"))) {
      // Water - extremely reflective
      Props.AbsorptionCoefficient = 0.01f; // Extremely reflective
      Props.Diffusion = 0.1f;
      Props.Dampening = 0.2f;
    }
    else if (MatStr.Contains(TEXT("EvoGrass"))) {
      // Grass - moderately absorptive
      Props.AbsorptionCoefficient = 0.4f;
      Props.Diffusion = 0.7f;
      Props.Dampening = 0.6f;
    }
    else if (MatStr.Contains(TEXT("EvoDirt"))) {
      // Dirt - moderately absorptive
      Props.AbsorptionCoefficient = 0.35f;
      Props.Diffusion = 0.6f;
      Props.Dampening = 0.5f;
    }
    else if (MatStr.Contains(TEXT("EvoSand"))) {
      // Sand - moderately absorptive, similar to dirt but slightly more
      Props.AbsorptionCoefficient = 0.38f;
      Props.Diffusion = 0.65f;
      Props.Dampening = 0.55f;
    }
    else if (MatStr.Contains(TEXT("EvoWood"))) {
      // Wood - medium absorption
      Props.AbsorptionCoefficient = 0.15f;
      Props.Diffusion = 0.5f;
      Props.Dampening = 0.4f;
    }
    else if (MatStr.Contains(TEXT("EvoRubber"))) {
      // Rubber - absorptive
      Props.AbsorptionCoefficient = 0.5f;
      Props.Diffusion = 0.6f;
      Props.Dampening = 0.7f;
    }
    else if (MatStr.Contains(TEXT("EvoSnow"))) {
      // Snow - reflective but with some absorption
      Props.AbsorptionCoefficient = 0.12f;
      Props.Diffusion = 0.4f;
      Props.Dampening = 0.35f;
    }
    // Default for unknown materials
    else {
      // Medium default values for any non-Evo materials
      Props.AbsorptionCoefficient = 0.3f;
      Props.Diffusion = 0.5f;
      Props.Dampening = 0.5f;
    }

    return Props;
  }
};

