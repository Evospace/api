// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "ContextProvider.h"
#include "Qr/CoordinameMinimal.h"
#include "Qr/Prototype.h"
#include "RailTypes.generated.h"

class URailStationBlockLogic;
class UInventory;
class UTrainSchedule;
class ULogicContext;

// Shared fixed-point scale for rail distances and train kinematics.
static constexpr int64 RailDistanceFixedScale = 1000;

UENUM()
enum class ETrainSimState : uint8 {
  Idle,
  Planning,
  Moving,
  Arrived,
  NoPath,
};

USTRUCT(BlueprintType)
struct FRailPathStep {
  GENERATED_BODY()

  UPROPERTY(VisibleAnywhere, Category = "Debug|Rail")
  FQrVector3i From = FQrVector3i::Zero();

  UPROPERTY(VisibleAnywhere, Category = "Debug|Rail")
  FQrVector3i To = FQrVector3i::Zero();
};

UCLASS(BlueprintType, Blueprintable)
class UTrainInstance : public UInstance, public ILogicContextProvider {
  GENERATED_BODY()

  public:
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug|Rail")
  TArray<FRailPathStep> Path;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug|Rail")
  int32 PathIndex = 0;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug|Rail")
  int64 DistanceAlongSegment = 0;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug|Rail")
  int64 Speed = 0;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug|Rail")
  ETrainSimState SimState = ETrainSimState::Idle;

  // Last simulation tick when SimState was updated.
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug|Rail")
  int64 SimTick = 0;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug|Rail")
  FString TargetStationIdentifier;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug|Rail")
  TWeakObjectPtr<URailStationBlockLogic> SourceStation;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug|Rail")
  TWeakObjectPtr<URailStationBlockLogic> TargetStation;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug|Rail")
  UInventory *Cargo = nullptr;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug|Rail")
  int32 InstanceRandom = 0;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rail|Schedule")
  UTrainSchedule *Schedule = nullptr;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rail|Schedule")
  int32 CurrentStopIndex = 0;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rail|Schedule")
  FString CurrentStationIdentifier;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rail|Schedule")
  ULogicContext *DepartureContext = nullptr;

  virtual ULogicContext *GetContext_Implementation() const override { return DepartureContext; }
};
