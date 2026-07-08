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

  /**
   * Removes the bound train and refunds one Train item to the owning player (skipped in
   * creative). Returns false when the widget lost its train. BP closes the widget on success.
   */
  UFUNCTION(BlueprintCallable, Category = "Train")
  bool RemoveTrainAndRefund();

  /** Live train instance re-resolved through the actor each call (safe after remote removals); null when the train is gone. */
  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Train")
  UTrainInstance *GetTrainInstance() const;

  /** Distinct station names, sorted; for the schedule-editing dropdown. */
  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Train")
  TArray<FString> GetStationNames() const;

  // One-node schedule editing for the train GUI: each call resolves the railway manager
  // and train index through the actor and runs the mutation as a local player action
  // (replicated to the session).

  /** Inserts a stop targeting StationName; InsertIndex -1 appends. */
  UFUNCTION(BlueprintCallable, Category = "Train|Schedule")
  bool AddScheduleStop(const FString &StationName, int32 InsertIndex = -1);

  /** Removes a stop; the last remaining stop cannot be removed. */
  UFUNCTION(BlueprintCallable, Category = "Train|Schedule")
  bool RemoveScheduleStop(int32 StopIndex);

  /** Reorders a stop to NewStopIndex. */
  UFUNCTION(BlueprintCallable, Category = "Train|Schedule")
  bool MoveScheduleStop(int32 StopIndex, int32 NewStopIndex);

  /** Retargets an existing stop to another station name. */
  UFUNCTION(BlueprintCallable, Category = "Train|Schedule")
  bool SetScheduleStopStation(int32 StopIndex, const FString &StationName);

  UFUNCTION(BlueprintCallable, Category = "Train|Schedule")
  bool SetScheduleLoop(bool bLoop);

  /** Publishes the stop's DepartureCondition after the GUI edited it in place. */
  UFUNCTION(BlueprintCallable, Category = "Train|Schedule")
  bool CommitScheduleStopCondition(int32 StopIndex);

  /** "Depart now": skips the departure condition at the next dispatch opportunity. */
  UFUNCTION(BlueprintCallable, Category = "Train|Schedule")
  bool ForceDepart();

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn = "true"))
  ATrainActor *TrainActor = nullptr;

  protected:
  UTrainInstance *TrainSimulation = nullptr;

  private:
  URailwayManager *ResolveManager(int32 &OutTrainIndex) const;
};
