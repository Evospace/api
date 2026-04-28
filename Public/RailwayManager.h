// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Qr/CoordinameMinimal.h"
#include "RailTypes.h"
#include "RailwayManager.generated.h"

class ADimension;
class URailNetwork;
class URailStationBlockLogic;
class ATrainActor;
class UTrainSchedule;
class UTrainInstance;
class FJsonObject;
class ARailSplineRenderManagerActor;

UCLASS()
class URailwayManager : public UObject {
  GENERATED_BODY()

  public:
  void Initialize(ADimension *Owner, URailNetwork *Network);
  virtual void BeginDestroy() override;

  void Tick(float SimStepSeconds);
  void TickVisual(float DeltaTime);

  bool SerializeJson(TSharedPtr<FJsonObject> Json) const;
  bool DeserializeJson(TSharedPtr<FJsonObject> Json);

  void RegisterStation(URailStationBlockLogic *Station);
  void UnregisterStation(URailStationBlockLogic *Station);
  URailStationBlockLogic *FindStationByID(const FString &ID) const;
  URailStationBlockLogic *FindStationByIdentifier(const FString &StationIdentifier) const;
  FString GetStationIdentifier(const URailStationBlockLogic *Station) const;
  const TMap<FString, URailStationBlockLogic *> &GetStations() const { return Stations; }

  FString GenerateStationID() const;

  bool LaunchTrain(URailStationBlockLogic *From, const FString &TargetStationIdentifier);
  bool SpawnTrainWithSchedule(UTrainSchedule *Schedule, int32 InitialStopIndex = 0);
  UTrainInstance *GetTrainDataMutable(int32 Index);
  const UTrainInstance *GetTrainData(int32 Index) const;
  UTrainInstance *GetDockedTrainAtStation(const FString &StationIdentifier) const;

  /** Centre + bogie poses for each car; anchor at path offset 0 = lead car centre (same as legacy single wagon). */
  bool TryGetTrainConsistWorldPoses(const UTrainInstance *Train, TArray<FTrainCarWorldPose> &OutPoses) const;

  /** Removes every train instance and its visual actor; clears station docking. */
  void ClearAllTrains();

  /** Reserved for graph-dependent behavior (e.g. UI refresh). */
  void OnRailGraphChanged();

#if WITH_EDITOR
  UFUNCTION(CallInEditor, Category = "Rail|Editor", meta = (DisplayName = "Delete all trains"))
  void EditorDeleteAllTrains();
#endif

  // When true, draws rail segment polylines in the world (URailwayManager on ADimension in PIE / game).
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug|Rail")
  bool bDebugDrawRailGraph = true;

  UPROPERTY(VisibleAnywhere, Category = "Debug|Rail")
  TArray<UTrainInstance *> Trains;

  // Parallel to Trains: visual actor per train (same index)
  UPROPERTY(VisibleAnywhere, Category = "Debug|Rail")
  TArray<ATrainActor *> TrainVisuals;

  // Train cruising speed in fixed uu per simulation tick.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rail|Simulation")
  int32 TrainMaxSpeedPerTick = 55000;

  // Train acceleration in fixed uu per simulation tick squared.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rail|Simulation")
  int32 TrainAccelerationPerTick = 250;

  // Train braking deceleration in fixed uu per simulation tick squared.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rail|Simulation")
  int32 TrainBrakingPerTick = 700;

  // Arrival tolerance in fixed uu before snapping to station.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rail|Simulation")
  int32 TrainStopDistanceTolerance = 5000;

  private:
  FQrVector3i StationRootKey(URailStationBlockLogic *S) const;
  bool BuildPathBetweenStations(const FString &SourceStationIdentifier, const FString &TargetStationIdentifier, TArray<FRailPathStep> &OutPath) const;
  bool TryDispatchTrainFromSchedule(int32 Index);
  bool IsDepartureConditionMet(const UTrainInstance *Train) const;
  void RefreshStationDocking();
  int64 ComputeRemainingPathDistance(const UTrainInstance *Train) const;
  void UpdateTrainMovement(int32 Index);
  void ArriveAtTarget(int32 Index, int32 ArrivedStopIndex);
  bool TryGetTrainTransform(const UTrainInstance *Train, FVector &OutWorldLocation, FRotator &OutWorldRotation) const;
  bool TryGetTrainCenterPose(const UTrainInstance *Train, FVector &OutCenterLocation, FVector &OutCenterTangent) const;
  bool TryGetTrainVisualPose(const UTrainInstance *Train, FVector &OutFrontBogieLocation, FVector &OutFrontBogieTangent, FVector &OutRearBogieLocation, FVector &OutRearBogieTangent) const;
  bool TrySampleTrainPathAtOffset(const UTrainInstance *Train, int64 OffsetFixed, FVector &OutWorldLocation, FVector &OutWorldTangent) const;
  void EnsureVisual(int32 Index);
  void ReleaseVisual(int32 Index);
  void DebugDrawRailGraph() const;
  void EnsureRailSplineRenderer();
  void ReleaseRailSplineRenderer();
  void RefreshRailSplineVisuals();

  UPROPERTY(VisibleAnywhere, Category = "Debug|Rail", meta = (AllowPrivateAccess = "true"))
  TWeakObjectPtr<ADimension> ownerDimension;

  UPROPERTY(VisibleAnywhere, Category = "Debug|Rail", meta = (AllowPrivateAccess = "true"))
  TWeakObjectPtr<URailNetwork> railNetwork;

  UPROPERTY(VisibleAnywhere, Category = "Debug|Rail", meta = (AllowPrivateAccess = "true"))
  TMap<FString, URailStationBlockLogic *> Stations;

  UPROPERTY(Transient)
  TWeakObjectPtr<ARailSplineRenderManagerActor> RailSplineRenderer;

  UPROPERTY(Transient)
  bool bRailSplineVisualDirty = true;

  float LastTrainSimStepSeconds = 1.0f / 20.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rail|Visual", meta = (AllowPrivateAccess = "true"))
  float TrainBogieSpacingUu = 220.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rail|Visual", meta = (AllowPrivateAccess = "true"))
  float TrainBogieZOffsetUu = -15.0f;
};
