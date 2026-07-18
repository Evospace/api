// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Qr/Vector.h"

/**
 * Deterministic per-block simulation PRNG (splitmix64).
 */
struct FSimRng {
  uint64 State = 0;

  // splitmix64 finalizer (mix64).
  static FORCEINLINE uint64 Scramble(uint64 x) {
    x ^= x >> 30;
    x *= 0xBF58476D1CE4E5B9ULL;
    x ^= x >> 27;
    x *= 0x94D049BB133111EBULL;
    x ^= x >> 31;
    return x;
  }

  static FORCEINLINE FSimRng SeedFor(uint64 SeedBase, const FQrVector3i &Pos) {
    const uint64 XY = (static_cast<uint64>(static_cast<uint32>(Pos.X)) << 32) |
                      static_cast<uint64>(static_cast<uint32>(Pos.Y));
    uint64 S = Scramble(SeedBase ^ XY);
    S = Scramble(S ^ static_cast<uint64>(static_cast<uint32>(Pos.Z)));
    return FSimRng{ S };
  }

  FORCEINLINE uint64 NextU64() {
    State += 0x9E3779B97F4A7C15ULL;
    return Scramble(State);
  }

  FORCEINLINE uint32 NextU32() { return static_cast<uint32>(NextU64() >> 32); }

  FORCEINLINE uint32 NextBelow(uint32 Bound) {
    return static_cast<uint32>((static_cast<uint64>(NextU32()) * Bound) >> 32);
  }

  FORCEINLINE int32 RandRange(int32 Min, int32 Max) {
    if (Max <= Min) {
      return Min;
    }
    return Min + static_cast<int32>(NextBelow(static_cast<uint32>(Max - Min + 1)));
  }

  FORCEINLINE bool Chance(int32 Percent) {
    return static_cast<int32>(NextBelow(100)) < Percent;
  }
};
