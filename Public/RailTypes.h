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

/** Centre-to-centre distance along the track for consecutive cars in a consist. */
static constexpr float RailTrainCarLengthUu = 400.0f;

/** Wheelbase: distance between the two bogies of one car. */
static constexpr float RailTrainCarBogieSpacingUu = 220.0f;

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

// World pose for one car (centre + two bogies), used for rendering and path debugging.
USTRUCT(BlueprintType)
struct FTrainCarWorldPose {
  GENERATED_BODY()

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rail|Visual")
  FVector CenterLocation = FVector::ZeroVector;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rail|Visual")
  FVector CenterTangent = FVector::ForwardVector;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rail|Visual")
  FVector FrontBogieLocation = FVector::ZeroVector;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rail|Visual")
  FVector FrontBogieTangent = FVector::ForwardVector;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rail|Visual")
  FVector RearBogieLocation = FVector::ZeroVector;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rail|Visual")
  FVector RearBogieTangent = FVector::ForwardVector;
};

// Basis for future path occupancy: longitudinal span from the lead car centre (sim anchor at path offset 0) along the last edge direction.
USTRUCT(BlueprintType)
struct FTrainConsistPathSpan {
  GENERATED_BODY()

  /** How far the front of the lead car is ahead of the sim anchor, along the track, fixed units. */
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rail|Physics")
  int64 LeadHeadForwardFromAnchorFixed = 0;

  /** How far the rear of the last car is behind the sim anchor, along the track, fixed units (non-negative). */
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rail|Physics")
  int64 LastTailAftFromAnchorFixed = 0;
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

  // Physical front end of the consist. Dispatch derives the route anchor from the train body.
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug|Rail")
  bool HasRailAnchor = false;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug|Rail")
  FQrVector3i RailAnchorFrom = FQrVector3i::Zero();

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug|Rail")
  FQrVector3i RailAnchorTo = FQrVector3i::Zero();

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug|Rail")
  int64 RailAnchorDistanceAlongSegment = 0;

  // Physical rear end of the consist. Dispatch derives the next leading-car centre from this point.
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug|Rail")
  bool HasRearRailAnchor = false;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug|Rail")
  FQrVector3i RearRailAnchorFrom = FQrVector3i::Zero();

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug|Rail")
  FQrVector3i RearRailAnchorTo = FQrVector3i::Zero();

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug|Rail")
  int64 RearRailAnchorDistanceAlongSegment = 0;

  /** Signed speed along the current path (fixed units per simulation tick); positive = toward path end, negative = toward path start. */
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug|Rail")
  int64 Speed = 0;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug|Rail")
  int32 BlockedByTrafficTicks = 0;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug|Rail")
  ETrainSimState SimState = ETrainSimState::Idle;

  // Last simulation tick when SimState was updated.
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug|Rail")
  int64 SimTick = 0;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug|Rail")
  bool bHasTargetStation = false;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug|Rail")
  FQrVector3i TargetStationPos = FQrVector3i::Zero();

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

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "Rail|Schedule")
  bool bDepartureForced = false;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rail|Schedule")
  bool bHasCurrentStation = false;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rail|Schedule")
  FQrVector3i CurrentStationPos = FQrVector3i::Zero();

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rail|Schedule")
  ULogicContext *DepartureContext = nullptr;

  // --- Consist / physical layout (single shared Cargo; per-car only for display & future path blocking)
  // Sim anchor: path offset 0 = centre of the lead (front) car, matching the previous single-wagon model.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rail|Consist", meta = (ClampMin = "1", UIMin = "1"))
  int32 NumCars = 4;

  int64 GetHeadHalfLengthFixed() const;
  int64 GetCarLengthFixed() const;
  int64 GetConsistLengthFromAnchorAftFixed() const;
  FTrainConsistPathSpan ComputePathSpan() const;

  virtual ULogicContext *GetContext_Implementation() const override { return DepartureContext; }
};
