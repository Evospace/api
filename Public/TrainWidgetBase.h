// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "TrainWidgetBase.generated.h"

class ATrainActor;
class UTrainInstance;
class URailwayManager;

UCLASS(BlueprintType)
class UTrainWidgetBase : public UUserWidget {
  GENERATED_BODY()

  public:
  virtual void SetTrainContext(UTrainInstance *InTrainSimulation, ATrainActor *InTrainActor);

  UTrainInstance *GetTrainSimulation() const { return TrainSimulation; }

  UFUNCTION(BlueprintCallable, Category = "Train")
  bool RemoveTrainAndRefund();

  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Train")
  UTrainInstance *GetTrainInstance() const;

  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Train")
  TArray<FString> GetStationNames() const;

  UFUNCTION(BlueprintCallable, Category = "Train|Schedule")
  bool AddScheduleStop(const FString &StationName, int32 InsertIndex = -1);

  UFUNCTION(BlueprintCallable, Category = "Train|Schedule")
  bool RemoveScheduleStop(int32 StopIndex);

  UFUNCTION(BlueprintCallable, Category = "Train|Schedule")
  bool MoveScheduleStop(int32 StopIndex, int32 NewStopIndex);

  UFUNCTION(BlueprintCallable, Category = "Train|Schedule")
  bool SetScheduleStopStation(int32 StopIndex, const FString &StationName);

  UFUNCTION(BlueprintCallable, Category = "Train|Schedule")
  bool SetScheduleLoop(bool bLoop);

  UFUNCTION(BlueprintCallable, Category = "Train|Schedule")
  bool CommitScheduleStopCondition(int32 StopIndex);

  UFUNCTION(BlueprintCallable, Category = "Train|Schedule")
  bool ForceDepart();

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn = "true"))
  ATrainActor *TrainActor = nullptr;

  protected:
  UTrainInstance *TrainSimulation = nullptr;

  private:
  URailwayManager *ResolveManager(int32 &OutTrainIndex) const;
};
