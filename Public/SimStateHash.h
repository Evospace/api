// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Hash/xxhash.h"
#include "Qr/Vector.h"

class UDimensionRuntime;
class UInventoryReader;

/**
 * Streaming xxHash64 writer for one hash scope (a block, a subnetwork, a train...).
 * Deterministic-sim state only: append raw value bits, never pointers, FName indices
 * or anything allocation-order dependent. Game thread only.
 */
struct FSimHashWriter {
  FXxHash64Builder Builder;

  FORCEINLINE void AppendBytes(const void *Data, uint64 Size) { Builder.Update(Data, Size); }

  template <typename T>
  FORCEINLINE void Append(const T &Value) {
    static_assert(std::is_trivially_copyable_v<T>, "hash input must be raw value bits");
    static_assert(!std::is_pointer_v<T>, "pointers are not stable across runs; hash a stable id instead");
    Builder.Update(&Value, sizeof(T));
  }

  FORCEINLINE void Append(const FQrVector3i &V) {
    Append(V.X);
    Append(V.Y);
    Append(V.Z);
  }

  void AppendString(const FString &S);
  /** Name as a stable identity; uncached — use for short-lived objects (block subobjects).
   * Hashes the plain string only: FName auto-numbers (NewObject without a name) depend on
   * global allocation order and would differ between identical runs. */
  void AppendPlainName(const FName &N);
  /** Stable identity of an immortal prototype (item, recipe, class): name hash cached per pointer.
   * Never pass objects that can be destroyed while the gate is on — the cache is keyed by pointer. */
  void AppendObjectName(const UObject *Object);
  /** Slot contents: item identity + count per slot. Null inventory hashes as a sentinel. */
  void AppendInventory(const UInventoryReader *Inventory);

  FORCEINLINE uint64 Finalize() { return Builder.Finalize().Hash; }
};

/** One desync-hash sample: whole-world hash plus per-section subhashes for bisecting a mismatch. */
struct FSimStateHashRecord {
  int64 Tick = 0;
  uint64 World = 0;
  uint64 Blocks = 0;
  uint64 Networks = 0;
  uint64 Trains = 0;
};

/** Trace mode: per-block hash at one sampled tick, for diffing two runs down to the exact block. */
struct FSimStateHashBlockTrace {
  int64 Tick = 0;
  FQrVector3i Pos = FQrVector3i(0);
  FName Class;
  uint64 Hash = 0;
};

/**
 * Desync-hash gate. Off by default; the only cost on the sim hot path is one bool branch
 * per substep. Enabled explicitly by the desync test harness (SimBench -SimBenchHashEveryN).
 * Game thread only.
 */
class FSimStateHash {
  public:
  static FORCEINLINE bool IsEnabled() { return GEnabled; }

  /** Start collecting: hash every Period-th substep from now. Resets records and the name cache. */
  static void Begin(int32 Period);
  static void End();

  /** Also record a per-block hash at every sampled tick (diagnostic; memory grows with blocks × samples). */
  static void SetTraceBlocks(bool bEnabled) { GTraceBlocks = bEnabled; }
  static bool IsTracingBlocks() { return GTraceBlocks; }
  static TArray<FSimStateHashBlockTrace> &GetBlockTraces() { return BlockTraces; }
  static bool WriteBlockTracesJson(const FString &Path);

  /** Called by UDimensionRuntime::RunSimSubstep after the sim state for this tick is final. */
  static void OnSubstepEnd(const UDimensionRuntime &Runtime);

  static const TArray<FSimStateHashRecord> &GetRecords() { return Records; }
  /** Order-sensitive combination of all record hashes; one number to compare two runs. */
  static uint64 FinalCombined();
  static bool WriteRecordsJson(const FString &Path);

  /** Stable per-session key for a prototype-like object: xxHash64 of its name. Cached by pointer. */
  static uint64 StableObjectKey(const UObject *Object);

  private:
  static bool GEnabled;
  static bool GTraceBlocks;
  static int32 HashPeriod;
  static int64 SubstepCounter;
  static TArray<FSimStateHashRecord> Records;
  static TArray<FSimStateHashBlockTrace> BlockTraces;
};
