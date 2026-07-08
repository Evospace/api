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

  /** Registers by block position; assigns a default name ("Station N") when unnamed. */
  void RegisterStation(URailStationBlockLogic *Station);
  void UnregisterStation(URailStationBlockLogic *Station);

  URailStationBlockLogic *FindStationAt(const FQrVector3i &Pos) const;

  /** All stations carrying Name, sorted by block position (deterministic across peers). */
  void FindStationsByName(const FString &Name, TArray<URailStationBlockLogic *> &Out) const;

  const TMap<FQrVector3i, URailStationBlockLogic *> &GetStations() const { return Stations; }

  /**
   * Renames a station. Renaming the last station carrying the old name also updates
   * every schedule stop that referenced it. Pc non-null marks a local player action
   * (replicated to the session); internal/remote callers pass nullptr.
   */
  bool RenameStation(APlayerController *Pc, URailStationBlockLogic *Station, const FString &NewName);

  /** Smallest free "Station N"; deterministic function of the registered set. */
  FString GenerateDefaultStationName() const;

  /** Distinct station names, sorted; for schedule-editing UI. */
  UFUNCTION(BlueprintCallable, Category = "Rail|Station")
  TArray<FString> GetAllStationNames() const;

  bool LaunchTrain(URailStationBlockLogic *From, const FString &TargetStationName);

  /**
   * InitialStation null = resolve by the initial stop's name (first free candidate in
   * deterministic order); non-null must carry the initial stop's name.
   */
  bool SpawnTrainWithSchedule(UTrainSchedule *Schedule, int32 InitialStopIndex = 0, int32 NumCars = 4, URailStationBlockLogic *InitialStation = nullptr);

  /**
   * Player-facing placement: spawns a consist docked at Station with a single-stop schedule.
   * Pc non-null marks a local player action (replicated to the session); internal/remote callers pass nullptr.
   * Returns the new train index or INDEX_NONE.
   */
  int32 PlaceTrainAtStation(APlayerController *Pc, URailStationBlockLogic *Station, int32 NumCars = 4);

  /**
   * Tears down one train: undock, release visual, drop the sim instance.
   * Pc non-null marks a local player action (replicated to the session).
   */
  bool RemoveTrain(APlayerController *Pc, int32 Index);

  UTrainInstance *GetTrainDataMutable(int32 Index);
  const UTrainInstance *GetTrainData(int32 Index) const;

  // --- Schedule editing (train GUI). Pc non-null marks a local player action
  // (replicated to the session); internal/remote callers pass nullptr. Every mutation
  // keeps CurrentStopIndex pointing at the same stop object (Factorio semantics).

  /** Inserts a stop targeting StationName with an Always departure condition; InsertIndex -1 appends. */
  UFUNCTION(BlueprintCallable, Category = "Rail|Schedule")
  bool AddScheduleStop(APlayerController *Pc, int32 TrainIndex, const FString &StationName, int32 InsertIndex = -1);

  /** Removes a stop. The last remaining stop cannot be removed (remove the train instead). */
  UFUNCTION(BlueprintCallable, Category = "Rail|Schedule")
  bool RemoveScheduleStop(APlayerController *Pc, int32 TrainIndex, int32 StopIndex);

  /** Reorders a stop to NewStopIndex (both must be valid indices). */
  UFUNCTION(BlueprintCallable, Category = "Rail|Schedule")
  bool MoveScheduleStop(APlayerController *Pc, int32 TrainIndex, int32 StopIndex, int32 NewStopIndex);

  /** Retargets an existing stop to another (non-empty) station name. */
  UFUNCTION(BlueprintCallable, Category = "Rail|Schedule")
  bool SetScheduleStopStation(APlayerController *Pc, int32 TrainIndex, int32 StopIndex, const FString &StationName);

  UFUNCTION(BlueprintCallable, Category = "Rail|Schedule")
  bool SetScheduleLoop(APlayerController *Pc, int32 TrainIndex, bool bLoop);

  /**
   * Publishes the stop's DepartureCondition to the session. The GUI edits the
   * UCondition object in place; call this once per completed edit.
   */
  UFUNCTION(BlueprintCallable, Category = "Rail|Schedule")
  bool CommitScheduleStopCondition(APlayerController *Pc, int32 TrainIndex, int32 StopIndex);

  /** Remote-apply counterpart of CommitScheduleStopCondition (net handler). */
  bool ApplyScheduleStopConditionJson(int32 TrainIndex, int32 StopIndex, const FString &ConditionJson);

  /** Departs at the next dispatch opportunity regardless of the departure condition. */
  UFUNCTION(BlueprintCallable, Category = "Rail|Schedule")
  bool ForceDepartTrain(APlayerController *Pc, int32 TrainIndex);

  /** Centre + bogie poses for each car; anchor at path offset 0 = lead car centre (same as legacy single wagon). */
  bool TryGetTrainConsistWorldPoses(const UTrainInstance *Train, TArray<FTrainCarWorldPose> &OutPoses) const;

  /** True if any two consist bodies geometrically overlap (gap 0). Smoke-test / debug invariant. */
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

  // Minimum along-track clearance kept between two consist bodies (and around shared graph nodes), uu.
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
    int64 &OutCost) const;
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
  /** 1-D body occupancy: true when TrainForConsist placed at (PathIndex, DistanceAlong) on Path would come closer than the follow gap to another consist (shared undirected edge interval or shared graph node). */
  bool WouldConsistBodyOverlapOtherTrains(int32 SelfIndex, const TArray<FRailPathStep> &Path, const UTrainInstance *TrainForConsist, int32 PathIndex, int64 DistanceAlong) const;
  /** True when CandidatePath traverses an edge that a currently moving train still has ahead of it in the opposite direction (head-on avoidance at dispatch). */
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

  // Keyed by the station block position — the stable identity; names are non-unique labels.
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
