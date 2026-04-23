// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrainActor.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class URailwayManager;
class UTrainHoverWidgetBase;
class UTrainWidgetBase;
class UHudWidget;
class UTrainInstance;

UCLASS()
class ATrainActor : public AActor {
  GENERATED_BODY()

  public:
  ATrainActor();
  virtual void BeginPlay() override;
  void BindToTrain(URailwayManager *InRailwayManager, int32 InTrainIndex);
  void ApplyRailPose(
    const FVector &CenterLocation,
    const FVector &CenterTangent,
    const FVector &FrontBogieLocation,
    const FVector &FrontBogieTangent,
    const FVector &RearBogieLocation,
    const FVector &RearBogieTangent);
  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Train")
  UTrainInstance *GetTrainSimulation() const;
  TSubclassOf<UTrainHoverWidgetBase> GetHoverWidgetClass() const;
  TSubclassOf<UTrainWidgetBase> GetWidgetClass() const;
  void OpenWidget(UHudWidget *HudWidget);

  UPROPERTY(VisibleAnywhere)
  USceneComponent *Root = nullptr;

  UPROPERTY(VisibleAnywhere)
  UStaticMeshComponent *Body = nullptr;

  private:
  UPROPERTY(VisibleAnywhere)
  UStaticMeshComponent *FrontWheelGroup = nullptr;

  UPROPERTY(VisibleAnywhere)
  UStaticMeshComponent *RearWheelGroup = nullptr;

  UPROPERTY(EditAnywhere, Category = "Train|Visual")
  float WagonBodyZOffset = 10.0f;

  UPROPERTY()
  URailwayManager *RailwayManager = nullptr;

  UPROPERTY()
  int32 TrainIndex = INDEX_NONE;
};
