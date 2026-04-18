// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "WeatherTypes.generated.h"

USTRUCT(BlueprintType)
struct FWeatherState {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather", meta = (ClampMin = "0.0", ClampMax = "1.0"))
  float Cloudiness01 = 0.0f;

  /** Rain / precipitation intensity from weather (0..1). Not surface wetness; wetness is tracked separately. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather", meta = (ClampMin = "0.0", ClampMax = "1.0"))
  float Precipitation01 = 0.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather", meta = (ClampMin = "0.0", ClampMax = "1.0"))
  float Fog01 = 0.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather", meta = (ClampMin = "0.0", ClampMax = "1.0"))
  float SecondFog01 = 0.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather", meta = (ClampMin = "0.0", ClampMax = "1.0"))
  float Storminess01 = 0.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather", meta = (ClampMin = "0.0"))
  float WindSpeed = 0.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather", meta = (ClampMin = "0.0", ClampMax = "1.0"))
  float GustAmount01 = 0.0f;

  // Wind heading angle in degrees around world Z axis.
  // 0 = +X direction, 90 = +Y direction.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
  float WindDirection = 0.0f;
};
