// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Qr/CoordinameMinimal.h"
#include "RailTypes.generated.h"

class URailStationBlockLogic;
class UInventory;

UENUM()
enum class ETrainSimState : uint8 {
  Idle,
  Planning,
  Moving,
  Arrived,
  NoPath,
};

USTRUCT()
struct FRailPathStep {
  GENERATED_BODY()

  UPROPERTY(VisibleAnywhere, Category = "Debug|Rail")
  FQrVector3i From = FQrVector3i::Zero();

  UPROPERTY(VisibleAnywhere, Category = "Debug|Rail")
  FQrVector3i To = FQrVector3i::Zero();
};

USTRUCT()
struct FTrainInstanceData {
  GENERATED_BODY()

  UPROPERTY(VisibleAnywhere, Category = "Debug|Rail")
  TArray<FRailPathStep> Path;

  UPROPERTY(VisibleAnywhere, Category = "Debug|Rail")
  int32 PathIndex = 0;

  UPROPERTY(VisibleAnywhere, Category = "Debug|Rail")
  float DistanceAlongSegment = 0.f;

  UPROPERTY(VisibleAnywhere, Category = "Debug|Rail")
  float Speed = 800.f;

  UPROPERTY(VisibleAnywhere, Category = "Debug|Rail")
  ETrainSimState SimState = ETrainSimState::Idle;

  UPROPERTY(VisibleAnywhere, Category = "Debug|Rail")
  int32 TargetStationId = 0;

  UPROPERTY(VisibleAnywhere, Category = "Debug|Rail")
  TWeakObjectPtr<URailStationBlockLogic> SourceStation;

  UPROPERTY(VisibleAnywhere, Category = "Debug|Rail")
  TWeakObjectPtr<URailStationBlockLogic> TargetStation;

  UPROPERTY(VisibleAnywhere, Category = "Debug|Rail")
  UInventory *Cargo = nullptr;

  UPROPERTY(VisibleAnywhere, Category = "Debug|Rail")
  int32 InstanceRandom = 0;
};
