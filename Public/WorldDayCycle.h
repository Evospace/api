// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace evo::WorldDayCycle {

/** Logical world-time clock rate; simulation TickRate does not change day-cycle progression. */
constexpr int32 WorldTimeTicksPerSecond = 20;

inline int64 PositiveModInt64(int64 a, int64 m) {
  check(m > 0);
  const int64 r = a % m;
  return r < 0 ? r + m : r;
}

/**
 * Phase within one day [0, len), len = max(1, DayLengthTicks).
 * Deterministic: only int64 tick state and int64 parameters.
 */
inline int64 DayPhaseTicks(int64 TotalGameTicks, int64 DayLengthTicks, int64 StartPhaseTicks) {
  const int64 len = FMath::Max<int64>(1, DayLengthTicks);
  return PositiveModInt64(TotalGameTicks + StartPhaseTicks, len);
}

/** [0, 1) fraction of day — use for rendering/MPC only. */
inline float TimeOfDay01Cosmetic(int64 PhaseTicks, int64 DayLengthTicks) {
  const int64 len = FMath::Max<int64>(1, DayLengthTicks);
  return static_cast<float>(PhaseTicks) / static_cast<float>(len);
}

/** [0, 24) hours — use for curves/lighting only. */
inline float TimeOfDayHoursCosmetic(int64 PhaseTicks, int64 DayLengthTicks) {
  return TimeOfDay01Cosmetic(PhaseTicks, DayLengthTicks) * 24.0f;
}

/** Frozen UI hour -> phase; float only on this cosmetic boundary. */
inline int64 LockedHoursToPhaseTicks(float LockedHours, int64 DayLengthTicks) {
  const int64 len = FMath::Max<int64>(1, DayLengthTicks);
  const float h = FMath::Fmod(FMath::Max(0.f, LockedHours), 24.f);
  const int64 t = static_cast<int64>(FMath::RoundToInt((h / 24.f) * static_cast<float>(len)));
  return PositiveModInt64(t, len);
}

inline float ResolveSessionTimeOfDayHoursCosmetic(
  bool bWorldTimeAutoAdvance,
  float LockedWorldTimeOfDayHours,
  int64 DayLengthTicks,
  int64 StartPhaseTicks,
  int64 TotalGameTicks) {
  if (!bWorldTimeAutoAdvance) {
    return LockedWorldTimeOfDayHours;
  }
  const int64 phase = DayPhaseTicks(TotalGameTicks, DayLengthTicks, StartPhaseTicks);
  return TimeOfDayHoursCosmetic(phase, DayLengthTicks);
}

} // namespace evo::WorldDayCycle
