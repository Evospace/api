// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "RailTypes.h"
#include "TrainActor.generated.h"

class USceneComponent;
class URailwayManager;
class UTrainHoverWidgetBase;
class UTrainWidgetBase;
class UHudWidget;
class UTrainInstance;

USTRUCT()
struct FEvospaceTrainCarMeshGroup {
  GENERATED_BODY()

  UPROPERTY(VisibleAnywhere)
  UStaticMeshComponent *CarBody = nullptr;

  UPROPERTY(VisibleAnywhere)
  UStaticMeshComponent *FrontWheel = nullptr;

  UPROPERTY(VisibleAnywhere)
  UStaticMeshComponent *RearWheel = nullptr;
};

UCLASS()
class ATrainActor : public AActor {
  GENERATED_BODY()

  public:
  ATrainActor();
  virtual void BeginPlay() override;
  void BindToTrain(URailwayManager *InRailwayManager, int32 InTrainIndex);
  void SyncCarVisuals(const UTrainInstance *TrainData);
  void ApplyConsistPose(const TArray<FTrainCarWorldPose> &CarPoses);
  void SetConsistPoseTarget(const TArray<FTrainCarWorldPose> &CarPoses, int64 SimTick, float SimStepSeconds);
  void UpdateConsistVisual(float DeltaTime);
  void ResetConsistPoseSmoothing();
  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Train")
  UTrainInstance *GetTrainSimulation() const;
  TSubclassOf<UTrainHoverWidgetBase> GetHoverWidgetClass() const;
  TSubclassOf<UTrainWidgetBase> GetWidgetClass() const;
  void OpenWidget(UHudWidget *HudWidget);

  UPROPERTY(VisibleAnywhere)
  USceneComponent *Root = nullptr;

  // First car body; kept for direct BP references. Updated by SyncCarVisuals.
  UPROPERTY(VisibleAnywhere)
  UStaticMeshComponent *Body = nullptr;

  private:
  void EnsureCarCount(int32 NumCars);
  UStaticMeshComponent *AddMeshComponent(
    FName Suffix, UStaticMesh *Mesh, bool bQueryCollision);
  UStaticMesh *ResolveWagonMesh() const;
  UStaticMesh *ResolveWheelMesh() const;
  static FTrainCarWorldPose InterpolateCarPose(
    const FTrainCarWorldPose &From, const FTrainCarWorldPose &To, float Alpha);

  UPROPERTY(VisibleAnywhere)
  TArray<FEvospaceTrainCarMeshGroup> CarMeshGroups;

  TArray<FTrainCarWorldPose> PreviousCarPoses;
  TArray<FTrainCarWorldPose> TargetCarPoses;
  TArray<FTrainCarWorldPose> RenderedCarPoses;

  float PoseInterpolationElapsed = 0.0f;
  float PoseInterpolationDuration = 1.0f / 20.0f;
  int64 LastPoseSimTick = INDEX_NONE;
  bool HasPoseSmoothing = false;

  UPROPERTY(EditAnywhere, Category = "Train|Visual")
  float WagonBodyZOffset = 10.0f;

  UPROPERTY()
  URailwayManager *RailwayManager = nullptr;

  UPROPERTY()
  int32 TrainIndex = INDEX_NONE;
};
