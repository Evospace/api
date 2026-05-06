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

/** Integer-tick parameters for one surface; mirrors UStaticPlanet / WorldDayCycle. */
struct FSurfaceDayCycleParams {
  int64 DayLengthTicks = 144000;
  int64 StartPhaseTicks = 48000;
  int64 PhaseOffsetTicks = 0;

  /** Local day-phase tick for dawn anchor; same space as DayPhaseTicks modulo DayLengthTicks. */
  int64 DawnPhaseTicks = 24000;
  int64 SolarNoonPhaseTicks = 72000;
  int64 SunsetPhaseTicks = 120000;

  static FSurfaceDayCycleParams FromPlanet(const UStaticPlanet *Planet);
  static FSurfaceDayCycleParams FromGameSessionDefaults(const UGameSessionData *Session);
};
