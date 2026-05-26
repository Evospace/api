#pragma once

#include "CoreMinimal.h"
#include "HAL/PlatformTime.h"
#include "UObject/Object.h"

class UBlockLogic;
class UClass;
class UDimensionRuntime;

/** Factorio show-time-usage style simulation systems (ms per simulation tick). */
enum class EEvoSimPerfSystem : uint8 {
  SimTotal = 0,
  ResourceNetworks,
  ConveyorSystems,
  Drones,
  Railways,
  BlockAccessors,
  LogicIO,
  Count
};

struct FEvoSimPerfMsStat {
  float Mean = 0.f;
  float Min = 0.f;
  float Max = 0.f;
  float P50 = 0.f;
  float P95 = 0.f;
  float P99 = 0.f;
};

struct FEvoSimPerfFpsStat {
  float Mean = 0.f;
  float Min = 0.f;
  float Max = 0.f;
  float P5 = 0.f;
  float P50 = 0.f;
  float Low1Pct = 0.f;
  float Low01Pct = 0.f;
};

struct FEvoSimPerfEntityReport {
  UClass *Class = nullptr;
  FEvoSimPerfMsStat Timing;
  int32 Count = 0;
};

struct FEvoSimPerfReport {
  FEvoSimPerfMsStat Systems[static_cast<int32>(EEvoSimPerfSystem::Count)];
  TArray<FEvoSimPerfEntityReport> Entities;
  int64 SimulationTicks = 0;
};

struct FEvoSimPerfSessionMeta {
  int32 Version = 1;
  FString MapName;
  int32 WarmupSec = 0;
  int32 MeasureSec = 0;
  int32 SampleHz = 4;
  int32 TickRate = 20;
  int32 Logics = 0;
  int32 ResourceNetworks = 0;
  int32 ConveyorNetworks = 0;
  int64 SimTicks = 0;
  int64 ProfileTicks = 0;
  double MeasureElapsedSec = 0.0;
};

class FEvoSimPerfMetrics {
  public:
  static FEvoSimPerfMetrics &Get();

  static bool IsEnabled();
  static void SetEnabled(bool bEnabled);

  /** Starts a stress session capture window (measure phase). Resets prior samples. */
  static void BeginSession(double MeasureStartWallSec);
  static void EndSession();
  static bool IsSessionActive();

  /** Wall-clock frame sample during measure (StressSampleHz). */
  void RecordFrameSample(double MeasureElapsedSec, float SimTickMs, float FrameTimeMs);

  void Reset();
  void BuildReport(const UDimensionRuntime *Runtime, FEvoSimPerfReport &OutReport) const;
  FEvoSimPerfMsStat GetFrameTickSummary() const;
  FEvoSimPerfMsStat GetFrameTimeSummary() const;
  FEvoSimPerfFpsStat GetFpsSummary() const;
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
    if (!IsEnabled()) {
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
  struct FEvoSimPerfEntitySlot {
    uint64 TotalCycles = 0;
    uint64 LastTickCycles = 0;
    float TickMinMs = 0.f;
    float TickMaxMs = 0.f;
  };

  struct FEvoSimPerfSimTickSample {
    double TimeSec = 0.0;
    int64 SimTick = 0;
    float SystemsMs[static_cast<int32>(EEvoSimPerfSystem::Count)] = {};
  };

  void AddSystemCycles(EEvoSimPerfSystem System, uint64 Cycles);
  void AddEntityCycles(UClass *Class, uint64 Cycles);
  void CommitTickSample();

  static FEvoSimPerfMsStat BuildMsStat(uint64 TotalCycles, int64 TickCount, float PerTickMinMs, float PerTickMaxMs);
  static FEvoSimPerfMsStat BuildMsStatFromSamples(const TArray<float> &Samples);
  static FEvoSimPerfFpsStat BuildFpsStatFromFrameTimes(const TArray<float> &FrameTimesMs);
  static float PercentileFromSorted(const TArray<float> &Sorted, double Percentile);

  uint64 SystemCycles[static_cast<int32>(EEvoSimPerfSystem::Count)] = {};
  TMap<UClass *, FEvoSimPerfEntitySlot> EntitySlots;

  uint64 LastTickSystemCycles[static_cast<int32>(EEvoSimPerfSystem::Count)] = {};

  float SystemTickMinMs[static_cast<int32>(EEvoSimPerfSystem::Count)] = {};
  float SystemTickMaxMs[static_cast<int32>(EEvoSimPerfSystem::Count)] = {};

  int64 SimulationTickSamples = 0;

  bool bSessionActive = false;
  double SessionMeasureStartWallSec = 0.0;
  TArray<double> FrameTimeSec;
  TArray<float> FrameTickMs;
  TArray<float> FrameRenderMs;
  TArray<FEvoSimPerfSimTickSample> SimTickSamples;
};
