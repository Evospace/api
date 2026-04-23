// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "TrainWidgetBase.generated.h"

class ATrainActor;
class UTrainInstance;

UCLASS(BlueprintType)
class UTrainWidgetBase : public UUserWidget {
  GENERATED_BODY()

  public:
  virtual void SetTrainContext(UTrainInstance *InTrainSimulation, ATrainActor *InTrainActor);

  UTrainInstance *GetTrainSimulation() const { return TrainSimulation; }

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn = "true"))
  ATrainActor *TrainActor = nullptr;

  protected:
  UTrainInstance *TrainSimulation = nullptr;
};
