// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include <Subsystems/GameInstanceSubsystem.h>
#include "Containers/Ticker.h"
#include <Engine/EngineBaseTypes.h>

#include "SmokeTestSubsystem.generated.h"

/**
 * Minimal Shipping-friendly smoke test runner.
 * Enabled via command line flag: -SmokeTest
 *
 * Criteria (initial version):
 * - Mod loader content finished loading (UModLoadingSubsystem::IsContentLoaded)
 * - First tick of a Game world after BeginPlay
 *
 * On success exits with status 0, on timeout exits with status 1.
 */
UCLASS()
class USmokeTestSubsystem : public UGameInstanceSubsystem {
  GENERATED_BODY()

  public:
  virtual void Initialize(FSubsystemCollectionBase &Collection) override;
  virtual void Deinitialize() override;

  private:
  bool SmokeTick(float DeltaSeconds);
  void OnWorldTickStart(UWorld *World, ELevelTick TickType, float DeltaSeconds);
  bool RunPostLoadCoreChecks(UWorld *World);
  bool HasSpawnedColumns(UWorld *World, int32 &OutColumnCount, int32 &OutWallCount) const;

  enum class ESmokeStage : uint8 {
    WaitingForMainGameInstance = 0,
    WaitingForModsLoaded,
    WaitingBeforeOpenMap,
    WaitingForTargetMapTick,
    RunningAfterFirstTick,
  };

  bool Enabled = false;
  bool ModsLoaded = false;
  bool LoadRequested = false;
  bool LoadedWorldTicked = false;

  double StartSeconds = 0.0;
  double LoadTimeoutSeconds = 30.0;
  double TimeoutSeconds = 90.0;
  double ExitDelaySeconds = 30.0;
  double ExitAtSeconds = 0.0;
  double StartLoadAtSeconds = 0.0;

  double ColumnsTimeoutSeconds = 10.0;
  double ColumnsStartSeconds = 0.0;

  double PostLoadCoreChecksAtSeconds = 0.0;
  double LastColumnsLogSeconds = 0.0;

  bool PostLoadCoreChecksDone = false;
  bool PostLoadCoreChecksPassed = false;
  bool PostLoadColumnsReady = false;

  TWeakObjectPtr<UWorld> LoadedWorld;
  FString LoadedMapName;

  ESmokeStage Stage = ESmokeStage::WaitingForMainGameInstance;

  FString SmokeGameName;
  FString MapNameBeforeLoad;

  FTSTicker::FDelegateHandle TickHandle;
  FDelegateHandle WorldTickHandle;
};
