// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HAL/PlatformTime.h"
#include "UObject/Object.h"

class UClass;
class UDimensionRuntime;

/**
 * Simulation performance metrics (SimBench).
 *
 * Pure-sim, tick-exact profiling: per-system cycle counters sampled once per
 * simulation tick, aggregated into distributions at export time. Owns no
 * render/FPS/frame-time state by design — the bench drives the sim headless at
 * max throughput and the headline metric is sustained TPS + tick_ms percentiles.
 *
 * Two detail levels (see FEvoSimPerfMetrics::SetEntityTimingEnabled):
 *   - systems  (default): only per-system timing, minimal perturbation.
 *   - entities: additionally attribute time per UBlockLogic class.
 */
enum class EEvoSimPerfSystem : uint8 {
  SimTotal = 0,
  ResourceNetworks,
  ConveyorSystems,
  Railways,
  BlockAccessors,
  LogicIO,
  Count
};

/** Full per-tick distribution of a value (milliseconds). */
struct FEvoSimPerfDist {
  int32 Samples = 0;
  float Mean = 0.f;
  float P50 = 0.f;
  float P95 = 0.f;
  float P99 = 0.f;
  float Max = 0.f;
};

/** A single expensive tick, with its per-system breakdown (spike hunting). */
struct FEvoSimPerfWorstTick {
  int64 TickIndex = 0;
  float TotalMs = 0.f;
  float SystemsMs[static_cast<int32>(EEvoSimPerfSystem::Count)] = {};
  bool bGc = false;
};

/** Per-block-class attribution (detail=entities only). */
struct FEvoSimPerfEntityDist {
  UClass *Class = nullptr;
  int32 Count = 0;
  FEvoSimPerfDist Timing;
};

struct FEvoSimPerfReport {
  FEvoSimPerfDist Systems[static_cast<int32>(EEvoSimPerfSystem::Count)];
  TArray<FEvoSimPerfEntityDist> Entities;
  int64 MeasuredTicks = 0;
  int32 GcCount = 0;
};

/** Session metadata; the C++ fields, git/machine meta is added by the python runner. */
struct FEvoSimPerfSessionMeta {
  static constexpr int32 kVersion = 2;

  FString Save; // save/workload name
  FString Detail; // "systems" | "entities"
  int32 TickRate = 20;
  int32 WarmupTicks = 0;
  int32 MeasureTicks = 0;
  int32 FrameBudgetMs = 0;

  // Workload shape captured at measure start.
  int32 Logics = 0;
  int32 ResourceNetworks = 0;
  int32 ConveyorNetworks = 0;

  // Filled in on export from the measured window.
  double MeasureWallSec = 0.0;
  double MaxTps = 0.0;

  // Environment.
  FString BuildConfig;
  FString Platform;
  FString Cpu;
};

class FEvoSimPerfMetrics {
  public:
  static FEvoSimPerfMetrics &Get();

  static bool IsEnabled();
  static void SetEnabled(bool bEnabled);

  /** detail=entities: additionally attribute per UBlockLogic class (perturbs timing). */
  static void SetEntityTimingEnabled(bool bEnabled);
  static bool IsEntityTimingEnabled();

  /** Begin/end the measure window. Resets all samples. */
  static void BeginSession();
  static void EndSession();
  static bool IsSessionActive();

  void Reset();

  /** Number of simulation ticks committed since the last Reset/BeginSession. */
  int64 GetMeasuredTicks() const { return SimulationTickSamples; }

  /** Mark that a GC happened; the next committed tick is annotated as GC-affected. */
  void MarkGcPending();

  void BuildReport(const UDimensionRuntime *Runtime, FEvoSimPerfReport &OutReport) const;
  FEvoSimPerfDist GetSystemDist(EEvoSimPerfSystem System) const;

  bool ExportSessionJson(const FString &Path, const FEvoSimPerfSessionMeta &Meta, const UDimensionRuntime *Runtime) const;

  static const TCHAR *SystemName(EEvoSimPerfSystem System);

  template <typename Fn>
  static void TimeSystem(EEvoSimPerfSystem System, Fn &&FnBody) {
    if (!IsEnabled()) {
      FnBody();
      return;
    }
    FEvoSimPerfMetrics &Metrics = Get();
    const uint64 Start = FPlatformTime::Cycles64();
    FnBody();
    Metrics.AddSystemCycles(System, FPlatformTime::Cycles64() - Start);
  }

  template <typename Fn>
  static void TimeEntity(UObject *Entity, Fn &&FnBody) {
    if (!IsEntityTimingEnabled()) {
      FnBody();
      return;
    }
    FEvoSimPerfMetrics &Metrics = Get();
    const uint64 Start = FPlatformTime::Cycles64();
    FnBody();
    if (Entity) {
      Metrics.AddEntityCycles(Entity->GetClass(), FPlatformTime::Cycles64() - Start);
    }
  }

  /** Commit one simulation tick's per-system deltas as a sample. */
  void OnSimulationTickCompleted();

  private:
  struct FEntitySlot {
    uint64 TotalCycles = 0;
    uint64 LastTickCycles = 0;
    TArray<float> TickMs; // per-committed-tick delta (entities mode only)
  };

  struct FSimTickSample {
    float SystemsMs[static_cast<int32>(EEvoSimPerfSystem::Count)] = {};
    bool bGc = false;
  };

  void AddSystemCycles(EEvoSimPerfSystem System, uint64 Cycles);
  void AddEntityCycles(UClass *Class, uint64 Cycles);
  void CommitTickSample();

  static FEvoSimPerfDist BuildDist(const TArray<float> &Samples);
  static float PercentileFromSorted(const TArray<float> &Sorted, double Percentile);

  uint64 SystemCycles[static_cast<int32>(EEvoSimPerfSystem::Count)] = {};
  uint64 LastTickSystemCycles[static_cast<int32>(EEvoSimPerfSystem::Count)] = {};
  TArray<float> SystemTickMs[static_cast<int32>(EEvoSimPerfSystem::Count)];

  TMap<UClass *, FEntitySlot> EntitySlots;

  int64 SimulationTickSamples = 0;
  int32 GcCount = 0;
  bool bGcPending = false;
  bool bSessionActive = false;

  TArray<FSimTickSample> SimTickSamples;
};
