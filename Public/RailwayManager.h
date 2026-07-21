// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Qr/CoordinameMinimal.h"
#include "RailTypes.h"
#include "RailNetwork.h"
#include "RailwayManager.generated.h"

class ADimension;
class URailNetwork;
class URailStationBlockLogic;
class ATrainActor;
class UTrainSchedule;
class UTrainInstance;
class FJsonObject;
class ARailSplineRenderManagerActor;
class APlayerController;

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

  URailStationBlockLogic *FindStationAt(const FQrVector3i &Pos) const;

  void FindStationsByName(const FString &Name, TArray<URailStationBlockLogic *> &Out) const;

  const TMap<FQrVector3i, URailStationBlockLogic *> &GetStations() const { return Stations; }

  bool RenameStation(APlayerController *Pc, URailStationBlockLogic *Station, const FString &NewName);

  FString GenerateDefaultStationName() const;

  UFUNCTION(BlueprintCallable, Category = "Rail|Station")
  TArray<FString> GetAllStationNames() const;

  bool LaunchTrain(URailStationBlockLogic *From, const FString &TargetStationName);

  bool SpawnTrainWithSchedule(UTrainSchedule *Schedule, int32 InitialStopIndex = 0, int32 NumCars = 4, URailStationBlockLogic *InitialStation = nullptr);

  int32 PlaceTrainAtStation(APlayerController *Pc, URailStationBlockLogic *Station, int32 NumCars = 4);

  bool RemoveTrain(APlayerController *Pc, int32 Index);

  UTrainInstance *GetTrainDataMutable(int32 Index);
  const UTrainInstance *GetTrainData(int32 Index) const;

  UFUNCTION(BlueprintCallable, Category = "Rail|Schedule")
  bool AddScheduleStop(APlayerController *Pc, int32 TrainIndex, const FString &StationName, int32 InsertIndex = -1);

  UFUNCTION(BlueprintCallable, Category = "Rail|Schedule")
  bool RemoveScheduleStop(APlayerController *Pc, int32 TrainIndex, int32 StopIndex);

  UFUNCTION(BlueprintCallable, Category = "Rail|Schedule")
  bool MoveScheduleStop(APlayerController *Pc, int32 TrainIndex, int32 StopIndex, int32 NewStopIndex);

  UFUNCTION(BlueprintCallable, Category = "Rail|Schedule")
  bool SetScheduleStopStation(APlayerController *Pc, int32 TrainIndex, int32 StopIndex, const FString &StationName);

  UFUNCTION(BlueprintCallable, Category = "Rail|Schedule")
  bool SetScheduleLoop(APlayerController *Pc, int32 TrainIndex, bool bLoop);

  UFUNCTION(BlueprintCallable, Category = "Rail|Schedule")
  bool CommitScheduleStopCondition(APlayerController *Pc, int32 TrainIndex, int32 StopIndex);

  bool ApplyScheduleStopConditionJson(int32 TrainIndex, int32 StopIndex, const FString &ConditionJson);

  UFUNCTION(BlueprintCallable, Category = "Rail|Schedule")
  bool ForceDepartTrain(APlayerController *Pc, int32 TrainIndex);

  /** Centre + bogie poses for each car; anchor at path offset 0 = lead car centre (same as legacy single wagon). */
  bool TryGetTrainConsistWorldPoses(const UTrainInstance *Train, TArray<FTrainCarWorldPose> &OutPoses) const;

  bool DebugAnyConsistHardOverlap() const;

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
  bool bDebugDrawRailGraph = false;

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

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rail|Simulation")
  float TrainFollowGapUu = 150.0f;

  private:
  UTrainInstance *GetTrainWithSchedule(int32 TrainIndex) const;
  FQrVector3i StationRootKey(const URailStationBlockLogic *S) const;
  bool BuildPathBetweenStations(URailStationBlockLogic *From, URailStationBlockLogic *To, TArray<FRailPathStep> &OutPath) const;
  bool BuildPathFromTrainToStation(
    const UTrainInstance *Train,
    const URailStationBlockLogic *TargetStation,
    TArray<FRailPathStep> &OutPath,
    int32 &OutPathIndex,
    int64 &OutDistanceAlong,
    int64 &OutCost,
    bool &OutReversesFacing) const;
  void SyncRailAnchorFromPathPosition(UTrainInstance *Train) const;
  bool EnsureRailAnchorForCurrentStop(UTrainInstance *Train) const;
  bool TryDispatchTrainFromSchedule(int32 Index);
  bool IsDepartureConditionMet(const UTrainInstance *Train) const;
  void DockTrainAtCurrentStation(UTrainInstance *Train);
  void UndockTrainFromCurrentStation(UTrainInstance *Train);
  void RebuildStationDockingFromTrainStates();
  int64 ComputeRemainingPathDistance(const UTrainInstance *Train) const;
  int64 ComputeAnchorOffsetAlongPathFixed(const UTrainInstance *Train) const;
  int64 ComputeTotalPathLengthFixed(const UTrainInstance *Train) const;
  int64 ComputeRemainingPathDistanceBackward(const UTrainInstance *Train) const;
  void SnapTrainAnchorToPathEnd(UTrainInstance *Train);
  void SnapTrainAnchorToPathBackwardStop(UTrainInstance *Train);
  bool TrySetTrainAnchorByPathOffset(UTrainInstance *Train, int64 AnchorOffsetFixed);
  bool WouldConsistBodyOverlapOtherTrains(int32 SelfIndex, const TArray<FRailPathStep> &Path, const UTrainInstance *TrainForConsist, int32 PathIndex, int64 DistanceAlong) const;
  bool WouldPathOpposeMovingTraffic(int32 SelfIndex, const TArray<FRailPathStep> &CandidatePath) const;
  int64 GetFollowGapFixed() const;
  int64 ClampSignedMoveDeltaForOccupancy(int32 Index, int64 DeltaDistanceFixed) const;
  /** Applies signed delta along Path; clamps at path ends. Returns applied distance (may be less than Delta if obstructed). */
  int64 MoveTrainAlongPath(UTrainInstance *Train, int64 DeltaDistanceFixed);
  void UpdateTrainMovement(int32 Index);
  void ArriveAtTarget(int32 Index, int32 ArrivedStopIndex);
  bool TryGetTrainTransform(const UTrainInstance *Train, FVector &OutWorldLocation, FRotator &OutWorldRotation) const;
  bool TryGetTrainCenterPose(const UTrainInstance *Train, FVector &OutCenterLocation, FVector &OutCenterTangent) const;
  bool TryGetTrainVisualPose(const UTrainInstance *Train, FVector &OutFrontBogieLocation, FVector &OutFrontBogieTangent, FVector &OutRearBogieLocation, FVector &OutRearBogieTangent) const;
  bool TrySampleTrainPathAtOffset(const UTrainInstance *Train, int64 OffsetFixed, FVector &OutWorldLocation, FVector &OutWorldTangent) const;
  void EnsureVisual(int32 Index);
  void ReleaseVisual(int32 Index);
  ATrainActor *FindVisualForTrain(const UTrainInstance *Train) const;
  void DebugDrawRailGraph() const;
  void EnsureRailSplineRenderer();
  void ReleaseRailSplineRenderer();
  void RefreshRailSplineVisuals();

  UPROPERTY(VisibleAnywhere, Category = "Debug|Rail", meta = (AllowPrivateAccess = "true"))
  TWeakObjectPtr<ADimension> ownerDimension;

  UPROPERTY(VisibleAnywhere, Category = "Debug|Rail", meta = (AllowPrivateAccess = "true"))
  TWeakObjectPtr<URailNetwork> railNetwork;

  UPROPERTY(VisibleAnywhere, Category = "Debug|Rail", meta = (AllowPrivateAccess = "true"))
  TMap<FQrVector3i, URailStationBlockLogic *> Stations;

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
