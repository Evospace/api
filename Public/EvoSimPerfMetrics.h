// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HAL/PlatformTime.h"
#include "UObject/Object.h"

class UClass;
class UDimensionRuntime;

enum class EEvoSimPerfSystem : uint8 {
  SimTotal = 0,
  ResourceNetworks,
  ConveyorSystems,
  Railways,
  BlockAccessors,
  LogicIO,
  Count
};

struct FEvoSimPerfDist {
  int32 Samples = 0;
  float Mean = 0.f;
  float P50 = 0.f;
  float P95 = 0.f;
  float P99 = 0.f;
  float Max = 0.f;
};

struct FEvoSimPerfWorstTick {
  int64 TickIndex = 0;
  float TotalMs = 0.f;
  float SystemsMs[static_cast<int32>(EEvoSimPerfSystem::Count)] = {};
  bool bGc = false;
};

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

struct FEvoSimPerfSessionMeta {
  static constexpr int32 kVersion = 2;

  FString Save;
  FString Detail;
  int32 TickRate = 20;
  int32 WarmupTicks = 0;
  int32 MeasureTicks = 0;
  int32 FrameBudgetMs = 0;

  int32 Logics = 0;
  int32 ResourceNetworks = 0;
  int32 ConveyorNetworks = 0;

  double MeasureWallSec = 0.0;
  double MaxTps = 0.0;

  FString BuildConfig;
  FString Platform;
  FString Cpu;
};

class FEvoSimPerfMetrics {
  public:
  static FEvoSimPerfMetrics &Get();

  static bool IsEnabled();
  static void SetEnabled(bool bEnabled);

  static void SetEntityTimingEnabled(bool bEnabled);
  static bool IsEntityTimingEnabled();

  static void BeginSession();
  static void EndSession();
  static bool IsSessionActive();

  void Reset();

  int64 GetMeasuredTicks() const { return SimulationTickSamples; }

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

  void OnSimulationTickCompleted();

  private:
  struct FEntitySlot {
    uint64 TotalCycles = 0;
    uint64 LastTickCycles = 0;
    TArray<float> TickMs;
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
