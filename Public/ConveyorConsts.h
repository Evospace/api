// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"

namespace ConveyorConsts {
static constexpr int32 SegmentLen = 96; // length of one segment (fixed-point units)

// Per-tier steps that divide SegmentLen exactly to avoid remainders
static constexpr int32 NumTierSteps = 9;
static constexpr int32 StepsByLevel[NumTierSteps] = { 8, 12, 16, 24, 32, 48, 96, 96, 96 };

static constexpr int32 StepForLevel(int32 level) {
  const int32 clamped = level < 0 ? 0 : (level < NumTierSteps ? level : NumTierSteps - 1);
  return StepsByLevel[clamped];
}
} // namespace ConveyorConsts
