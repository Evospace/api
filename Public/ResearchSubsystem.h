// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include <Subsystems/GameInstanceSubsystem.h>
#include "Containers/Ticker.h"
#include "ResearchSubsystem.generated.h"

class UStaticResearch;
class UStaticItem;
class UAutosizeInventory;
class USingleSlotInventory;
class AMainPlayerController;
class UGameSessionData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FResearchQueueUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FResearchFinished, UStaticResearch *, Research);

UCLASS()
class UResearchSubsystem : public UGameInstanceSubsystem {
  GENERATED_BODY()

  public:
  virtual void Initialize(FSubsystemCollectionBase &Collection) override;
  virtual void Deinitialize() override;

  bool DeserializeFromPlayerJson(TSharedPtr<FJsonObject> json);
  bool SerializeToPlayerJson(TSharedPtr<FJsonObject> json) const;

  void Reset();

  UFUNCTION(BlueprintCallable)
  UStaticResearch *GetActiveResearch() const { return ActiveResearch; }

  UFUNCTION(BlueprintCallable, BlueprintPure)
  int64 GetActiveResearchComplexity() const;

  UFUNCTION(BlueprintCallable, BlueprintPure)
  int64 GetActiveResearchLeft() const { return ActiveResearchLeft; }

  UFUNCTION(BlueprintCallable)
  void AddUnlockedItem(const UStaticItem *Item);

  // Core research API (controller should delegate)
  UFUNCTION(BlueprintCallable)
  void DequeueResearch();

  UFUNCTION(BlueprintCallable)
  bool CanEnqueueResearch(UStaticResearch *Research) const;

  UFUNCTION(BlueprintCallable)
  void EnqueueResearch(UStaticResearch *Research);

  UFUNCTION(BlueprintCallable)
  void FillCanEnqueue(UStaticResearch *EqnuedRes) const;

  UFUNCTION(BlueprintCallable)
  void SetActiveResearch(UStaticResearch *Research);

  UFUNCTION(BlueprintCallable)
  void CompleteResearch(UStaticResearch *Research);

  UFUNCTION(BlueprintCallable, BlueprintPure)
  float GetResearchProgress() const;

  UFUNCTION(BlueprintCallable, BlueprintPure)
  float GetResearchApprox() const;

  UFUNCTION(BlueprintCallable)
  TArray<UStaticResearch *> RequiredBy(UStaticResearch *Research) const;

  UFUNCTION(BlueprintCallable)
  TArray<UStaticResearch *> Requires(UStaticResearch *Research) const;

  bool HasAllRequired(UStaticResearch *Research) const;
  bool HasAllRequiredWithQueue(UStaticResearch *Research) const;

  UFUNCTION(BlueprintCallable)
  TArray<UStaticResearch *> &GetAllResearches() const;

  UFUNCTION(BlueprintCallable)
  TArray<UStaticResearch *> GetAllResearchesSorted() const;

  UFUNCTION(BlueprintCallable)
  int64 ApplyResearchPoints(int64 Points) {
    ActiveResearchLeft -= Points;
    if (ActiveResearchLeft < 0) {
      ActiveResearchLeft = 0;
    }
    return ActiveResearchLeft;
  }

  UFUNCTION(BlueprintCallable)
  TArray<UStaticResearch *> GetAllResearchesSortedFiltered(const FString &Filter) const;

  UFUNCTION(BlueprintCallable)
  const TArray<UStaticResearch *> &GetResearchQueue() const { return ResearchQueue; }

  UFUNCTION(BlueprintCallable)
  bool IsUnlocked(const UStaticItem *Item) const;

  // Called regularly to update progress and auto-complete
  void TickResearch(float DeltaSeconds);

  UFUNCTION()
  void InitializeResearchTreeOnStart(UGameSessionData *gameSessionData);

  // Event for UI
  UPROPERTY(BlueprintAssignable)
  FResearchQueueUpdated OnResearchQueueUpdated;

  UPROPERTY(BlueprintAssignable)
  FResearchFinished OnResearchFinished;

  UPROPERTY(EditAnywhere)
  TSet<const UStaticItem *> UnlockedItems;

  private:
  void EnsureCachesComputed() const;

  void CompleteResearch_Internal(UStaticResearch *Research);

  UPROPERTY(EditAnywhere)
  UStaticResearch *ActiveResearch = nullptr;

  UPROPERTY(EditAnywhere)
  int64 ActiveResearchLeft = 0;

  UPROPERTY(EditAnywhere)
  int64 ScienceLast = 0;

  UPROPERTY(EditAnywhere)
  int64 ScienceMedian = 0;

  UPROPERTY(EditAnywhere)
  float ScienceSampleTimer = 2.f;

  UPROPERTY(EditAnywhere)
  TArray<UStaticResearch *> ResearchQueue;

  UPROPERTY(EditAnywhere)
  TSet<UStaticResearch *> CompletedResearches;

  UPROPERTY(EditAnywhere)
  TMap<FName, int64> OldResearches;

  UPROPERTY(EditAnywhere)
  mutable TArray<UStaticResearch *> AllResearchesCache;

  FTSTicker::FDelegateHandle TickDelegateHandle;
};
