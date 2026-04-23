// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "RailTypes.h"
#include "RailwayManager.generated.h"

class ADimension;
class URailNetwork;
class URailStationBlockLogic;
class ATrainActor;

UCLASS()
class URailwayManager : public UObject {
  GENERATED_BODY()

  public:
  void Initialize(ADimension *Owner, URailNetwork *Network);

  void Tick(float SimStepSeconds);
  void TickVisual(float DeltaTime);

  void RegisterStation(URailStationBlockLogic *Station);
  void UnregisterStation(URailStationBlockLogic *Station);
  URailStationBlockLogic *FindStationByID(int32 ID) const;
  const TMap<int32, URailStationBlockLogic *> &GetStations() const { return Stations; }

  int32 GenerateStationID();

  bool LaunchTrain(URailStationBlockLogic *From, int32 TargetStationId);

  /** Reserved for graph-dependent behavior (e.g. UI refresh); dispatch is inventory-driven. */
  void OnRailGraphChanged();

  /** When true, each rail station with tick tries to launch a train while input has items (first reachable other station), similar to drone stations. */
  UPROPERTY(EditAnywhere, Category = "Rail|Auto")
  bool bEnableAutoLaunch = true;

  // When true, draws rail segment polylines in the world (URailwayManager on ADimension in PIE / game).
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug|Rail")
  bool bDebugDrawRailGraph = true;

  UPROPERTY(VisibleAnywhere, Category = "Debug|Rail")
  TArray<FTrainInstanceData> Trains;

  // Parallel to Trains: visual actor per train (same index)
  UPROPERTY(VisibleAnywhere, Category = "Debug|Rail")
  TArray<ATrainActor *> TrainVisuals;

  private:
  FQrVector3i StationRootKey(URailStationBlockLogic *S) const;
  void UpdateTrainMovement(int32 Index, float Dt);
  void ArriveAtTarget(int32 Index);
  bool TryGetTrainVisualTransform(const FTrainInstanceData &T, FVector &OutWorldLocation, FRotator &OutWorldRotation) const;
  void EnsureVisual(int32 Index);
  void ReleaseVisual(int32 Index);
  void DebugDrawRailGraph() const;

  UPROPERTY(VisibleAnywhere, Category = "Debug|Rail", meta = (AllowPrivateAccess = "true"))
  TWeakObjectPtr<ADimension> ownerDimension;

  UPROPERTY(VisibleAnywhere, Category = "Debug|Rail", meta = (AllowPrivateAccess = "true"))
  TWeakObjectPtr<URailNetwork> railNetwork;

  UPROPERTY(VisibleAnywhere, Category = "Debug|Rail", meta = (AllowPrivateAccess = "true"))
  TMap<int32, URailStationBlockLogic *> Stations;

  UPROPERTY(VisibleAnywhere, Category = "Debug|Rail", meta = (AllowPrivateAccess = "true"))
  int32 NextStationId = 1;
};
