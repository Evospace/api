// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Qr/Vector.h"

/**
 * Deterministic per-block simulation PRNG (splitmix64).
 *
 * 8 bytes of state, ~5 integer ops per draw, bit-identical across MSVC/clang and
 * x64/arm64 (no 128-bit multiply, no libc rand()). Use only for simulation
 * randomness that must stay in lockstep between peers and survive save/reload;
 * never for presentation (weather, audio, drone sway, ...).
 *
 * Seed a stream with SeedFor from (world seed, surface, block position) so identical
 * worlds reproduce identical draws regardless of spawn/BeginPlay/tick order. The
 * owning block persists State in Serialize/DeserializeJson and folds it into the
 * desync hash, so any RNG drift is caught directly.
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

  // Position-dependent stream seed. Distinct positions (and distinct surfaces via
  // SeedBase) yield independent streams; order of iteration is irrelevant.
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

  // Uniform in [0, Bound) via Lemire's multiply-shift: no rejection loop, so it is
  // constant-work and fully deterministic. Bound == 0 returns 0.
  FORCEINLINE uint32 NextBelow(uint32 Bound) {
    return static_cast<uint32>((static_cast<uint64>(NextU32()) * Bound) >> 32);
  }

  // Inclusive [Min, Max], matching FMath::RandRange(int32, int32).
  FORCEINLINE int32 RandRange(int32 Min, int32 Max) {
    if (Max <= Min) {
      return Min;
    }
    return Min + static_cast<int32>(NextBelow(static_cast<uint32>(Max - Min + 1)));
  }

  // True with probability Percent/100; always consumes exactly one draw.
  FORCEINLINE bool Chance(int32 Percent) {
    return static_cast<int32>(NextBelow(100)) < Percent;
  }
};
