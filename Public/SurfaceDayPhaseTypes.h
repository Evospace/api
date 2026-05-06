// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SurfaceDayPhaseTypes.generated.h"

class UGameSessionData;
class UStaticPlanet;

/** Named anchors along the cosmetic 24h day; used by the per-surface day-phase bus. */
UENUM(BlueprintType)
enum class ESurfaceDayPhaseAnchor : uint8 {
  /** Phase tick wrapped to tick 0 (start of astronomical day). */
  Midnight UMETA(DisplayName = "Midnight"),

  Dawn UMETA(DisplayName = "Dawn"),
  SolarNoon UMETA(DisplayName = "Solar Noon"),
  Sunset UMETA(DisplayName = "Sunset"),
};

namespace evo::SurfaceDayPhaseThresholds {

// Cosmetic anchor hours [0, 24); aligned with lighting/curve samples (WeatherController MPC uses full 24h cosmetics).
constexpr float MidnightHour = 0.0f;
constexpr float DawnHour = 6.0f;
constexpr float SolarNoonHour = 12.0f;
constexpr float SunsetHour = 18.0f;

} // namespace evo::SurfaceDayPhaseThresholds

/** Integer-tick parameters for one surface; mirrors UStaticPlanet / WorldDayCycle. */
struct FSurfaceDayCycleParams {
  int64 DayLengthTicks = 144000;
  int64 StartPhaseTicks = 48000;
  int64 PhaseOffsetTicks = 0;

  static FSurfaceDayCycleParams FromPlanet(const UStaticPlanet *Planet);
  static FSurfaceDayCycleParams FromGameSessionDefaults(const UGameSessionData *Session);
};
