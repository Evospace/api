// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrainActor.generated.h"

class UStaticMeshComponent;
class USceneComponent;

UCLASS()
class ATrainActor : public AActor {
  GENERATED_BODY()

  public:
  ATrainActor();
  virtual void BeginPlay() override;
  UPROPERTY(VisibleAnywhere)
  USceneComponent *Root = nullptr;

  UPROPERTY(VisibleAnywhere)
  UStaticMeshComponent *Body = nullptr;
};
