// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RailNetwork.h"
#include "RailSplineRenderManagerActor.generated.h"

class USceneComponent;
class USplineMeshComponent;
class UStaticMesh;

UCLASS()
class ARailSplineRenderManagerActor : public AActor {
  GENERATED_BODY()

  public:
  ARailSplineRenderManagerActor();

  void RebuildRailSegments(const TArray<FRailRenderSegmentData> &Segments);

  protected:
  virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

  private:
  void ClearRailSegments();

  UPROPERTY(Transient)
  TArray<USplineMeshComponent *> RailSplineMeshes;

  UPROPERTY(Transient)
  UStaticMesh *RailSplineStaticMesh = nullptr;
};
