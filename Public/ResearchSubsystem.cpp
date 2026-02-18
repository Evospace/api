// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#include "ResearchSubsystem.h"

#include "Evospace/Gui/HudWidget.h"
#include "Evospace/JsonHelper.h"
#include "Public/AutosizeInventory.h"
#include "Public/GameSessionSubsystem.h"
#include "Public/SingleSlotInventory.h"
#include "Public/StaticResearch.h"
#include "Public/StaticItem.h"
#include "Evospace/Item/InventoryLibrary.h"
#include "Evospace/Player/MainPlayerController.h"
#include "Evospace/Player/NeiComponent.h"
#include "Qr/Ensure.h"
#include "Qr/ModLoadingSubsystem.h"
#include "Qr/QrFind.h"
#include "Public/MainGameInstance.h"
#include "Public/RecipeDictionary.h"
#include "Public/Recipe.h"
#include "Public/StaticResearchRecipe.h"
#include "UObject/UObjectIterator.h"
#include <Engine/Engine.h>
#include <EngineUtils.h>
#include "Kismet/GameplayStatics.h"

void UResearchSubsystem::Initialize(FSubsystemCollectionBase &Collection) {
  Super::Initialize(Collection);
}

void UResearchSubsystem::Deinitialize() {
  Super::Deinitialize();
}

bool UResearchSubsystem::DeserializeFromPlayerJson(TSharedPtr<FJsonObject> json) {
  if (!json)
    return false;

  // Clear queue before loading: TryFind for TArray only appends, so without this
  // a prior in-session state (e.g. from same GameInstance after load) would duplicate entries.
  ResearchQueue.Empty();

  json_helper::TryFind(json, TEXT("ActiveResearch"), ActiveResearch);
  json_helper::TryFind(json, TEXT("ResearchQueue"), ResearchQueue);
  json_helper::TryGet(json, TEXT("ActiveResearchLeft"), ActiveResearchLeft);
  json_helper::TryGet(json, TEXT("OldResearches"), OldResearches);

  TArray<FName> completed;
  json_helper::TryGet(json, TEXT("CompletedResearches"), completed);
  CompletedResearches.Empty(completed.Num());
  for (auto &kv : completed) {
    auto res = QrFind<UStaticResearch>(*kv.ToString());
    if (expect(res, "UResearchSubsystem::DeserializeFromPlayerJson: research not found ", kv.ToString())) {
      CompletedResearches.Add(res);
    }
  }

  // Initialize recipe locking state before applying completed researches
  InitializeRecipeLocking();

  // Integrate previously separate ApplyLoadedCompletedResearches logic here
  for (auto r : CompletedResearches) {
    if (expect(r, "UResearchSubsystem::DeserializeFromPlayerJson: research is nullptr in CompletedResearches")) {
      auto controller = Cast<AMainPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
      r->ApplyToController(controller, r->mLevel + 1);
      r->Type = EResearchStatus::Complete;
    }
  }

  // Recompute dependent statuses based on loaded completions
  for (auto r : GetAllResearches()) {
    if (r->Type == EResearchStatus::Complete)
      continue;

    bool anyMissingRequired = false;
    bool anyInQueue = false;
    for (auto req : r->RequiredResearch) {
      if (!CompletedResearches.Contains(req)) {
        if (ResearchQueue.Contains(req)) {
          anyInQueue = true;
        } else {
          anyMissingRequired = true;
          break;
        }
      }
    }

    if (anyMissingRequired) {
      r->Type = EResearchStatus::Closed;
    } else if (anyInQueue) {
      r->Type = EResearchStatus::CanEnqueue;
    } else if (HasAllRequired(r)) {
      r->Type = EResearchStatus::Opened;
    }
  }

  // Ensure researches that are in queue are marked as Queued (including active one)
  for (auto q : ResearchQueue) {
    if (q && q->Type != EResearchStatus::Complete) {
      q->Type = EResearchStatus::Queued;
    }
  }
  if (ActiveResearch && ActiveResearch->Type != EResearchStatus::Complete) {
    ActiveResearch->Type = EResearchStatus::Queued;
  }

  OnResearchQueueUpdated.Broadcast();

  // Update NeiComponent unlocked tab after loading researches
  auto controller = Cast<AMainPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
  if (controller && controller->mNei) {
    controller->mNei->UpdateUnlockedTab();
  }

  return true;
}

bool UResearchSubsystem::SerializeToPlayerJson(TSharedPtr<FJsonObject> json) const {
  if (!json)
    return false;

  json_helper::TrySet(json, TEXT("ActiveResearchLeft"), ActiveResearchLeft);
  json_helper::TrySet(json, TEXT("ActiveResearch"), ActiveResearch);
  json_helper::TrySet(json, TEXT("ResearchQueue"), ResearchQueue);
  json_helper::TrySet(json, TEXT("OldResearches"), OldResearches);
  json_helper::TrySet(json, TEXT("CompletedResearches"), CompletedResearches.Array());

  return true;
}

// removed legacy ApplyLoadedCompletedResearches; logic moved to DeserializeFromPlayerJson

void UResearchSubsystem::AddUnlockedItem(const UStaticItem *Item) {
  if (expect(Item, "UResearchSubsystem::AddUnlockedItem with nullptr Item")) {
    UnlockedItems.Add(Item);
  }
}

bool UResearchSubsystem::IsUnlocked(const UStaticItem *Item) const {
  return UnlockedItems.Contains(Item);
}

void UResearchSubsystem::DequeueResearch() {
  if (ensure(ResearchQueue.Num() > 0)) {
    auto r = ResearchQueue.Pop();
    if (r == ActiveResearch) {
      SetActiveResearch(nullptr);
    }
    for (auto allRes : GetAllResearches()) {
      if (allRes->Type != EResearchStatus::Complete) {
        if (allRes->RequiredResearch.Contains(r)) {
          allRes->Type = EResearchStatus::Closed;
        }
      }
    }
    // Recompute status for the dequeued research itself if not completed
    if (r && r->Type != EResearchStatus::Complete) {
      if (HasAllRequired(r)) {
        r->Type = EResearchStatus::Opened;
      } else if (HasAllRequiredWithQueue(r)) {
        r->Type = EResearchStatus::CanEnqueue;
      } else {
        r->Type = EResearchStatus::Closed;
      }
    }
    OnResearchQueueUpdated.Broadcast();
  }
}

bool UResearchSubsystem::CanEnqueueResearch(UStaticResearch *Research) const {
  if (!Research || ResearchQueue.Contains(Research)) {
    return false;
  }

  switch (Research->Type) {
  case EResearchStatus::Restricted:
  case EResearchStatus::Complete:
  case EResearchStatus::Closed:
    return false;
  default:
    return true;
  }
}

void UResearchSubsystem::FillCanEnqueue(UStaticResearch *EqnuedRes) const {
  for (auto allRes : GetAllResearches()) {
    if (allRes->Type == EResearchStatus::Closed) {
      if (allRes->RequiredResearch.Contains(EqnuedRes)) {
        if (HasAllRequiredWithQueue(allRes)) {
          allRes->Type = EResearchStatus::CanEnqueue;
        }
      }
    }
  }
}

void UResearchSubsystem::EnqueueResearch(UStaticResearch *Research) {
  if (!Research)
    return;
  if (ensure(ResearchQueue.Find(Research) == INDEX_NONE)) {
    ResearchQueue.Push(Research);
    // Mark newly enqueued research as Queued
    if (Research->Type != EResearchStatus::Complete) {
      Research->Type = EResearchStatus::Queued;
    }
    if (ActiveResearch == nullptr) {
      SetActiveResearch(Research);
    }

    FillCanEnqueue(Research);
    OnResearchQueueUpdated.Broadcast();
  }
}

void UResearchSubsystem::SetActiveResearch(UStaticResearch *Research) {
  if (Research != ActiveResearch) {
    if (Research == nullptr) {
      if (ActiveResearch) {
        OldResearches.Add(ActiveResearch->GetFName(), ActiveResearchLeft);
        ActiveResearchLeft = 0;
      }
      ActiveResearch = nullptr;
      return;
    }

    if (Research->Type != EResearchStatus::Closed) {
      if (ActiveResearch != nullptr) {
        OldResearches.Add(ActiveResearch->GetFName(), ActiveResearchLeft);
      }

      ActiveResearch = Research;

      // Restore saved remaining progress if present; otherwise initialize from complexity
      if (auto old_r = OldResearches.Find(Research->GetFName())) {
        ActiveResearchLeft = *old_r;
        OldResearches.Remove(Research->GetFName());
      } else {
        ActiveResearchLeft = Research->Complexity;
      }
      // Active research is always considered Queued
      if (ActiveResearch->Type != EResearchStatus::Complete) {
        ActiveResearch->Type = EResearchStatus::Queued;
      }
    }

    OnResearchQueueUpdated.Broadcast();
  }
}

void UResearchSubsystem::CompleteResearch_Internal(UStaticResearch *Research) {
  if (!expect(Research, "UResearchSubsystem::CompleteResearch_Internal: Research is nullptr")) {
    return;
  }

  CompletedResearches.Add(Research);
  Research->Type = EResearchStatus::Complete;

  for (auto r : GetAllResearches()) {
    if (r->Type != EResearchStatus::Complete) {
      if (r->RequiredResearch.Contains(Research)) {
        if (HasAllRequired(r)) {
          r->Type = EResearchStatus::Opened;
        }
      }
    }
  }

  auto controller = Cast<AMainPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
  if (expect_once(controller, "UResearchSubsystem::CompleteResearch_Internal: controller is nullptr")) {
    Research->ApplyToController(controller, 0);
  }
}

void UResearchSubsystem::CompleteResearch(UStaticResearch *Research) {
  if (!expect(Research, "UResearchSubsystem::CompleteResearch: Research is nullptr")) {
    return;
  }

  CompleteResearch_Internal(Research);

  OnResearchFinished.Broadcast(Research);

  if (ResearchQueue.Num() > 0) {
    ResearchQueue.RemoveAt(0);
    if (ResearchQueue.Num() > 0) {
      auto nextR = ResearchQueue[0];
      SetActiveResearch(nextR);
    } else {
      ActiveResearch = nullptr;
    }
  }
}

int64 UResearchSubsystem::GetActiveResearchComplexity() const {
  return ActiveResearch ? ActiveResearch->Complexity : 0;
}

float UResearchSubsystem::GetResearchProgress() const {
  if (!ActiveResearch)
    return 0.f;

  auto complexity = FMath::Max(1, ActiveResearch->Complexity);
  return 1.f - ActiveResearchLeft / static_cast<float>(complexity);
}

float UResearchSubsystem::GetResearchApprox() const {
  return ActiveResearchLeft / static_cast<float>(ScienceMedian + 1) * 4;
}

TArray<UStaticResearch *> UResearchSubsystem::RequiredBy(UStaticResearch *Research) const {
  TArray<UStaticResearch *> arr;
  if (Research) {
    for (auto r : GetAllResearches()) {
      if (r->RequiredResearch.Contains(Research)) {
        arr.Add(r);
      }
    }
  }
  return arr;
}

TArray<UStaticResearch *> UResearchSubsystem::Requires(UStaticResearch *Research) const {
  if (Research) {
    return Research->RequiredResearch;
  }
  return {};
}

bool UResearchSubsystem::HasAllRequired(UStaticResearch *Research) const {
  for (auto r2 : Research->RequiredResearch) {
    if (!CompletedResearches.Contains(r2))
      return false;
  }
  return true;
}

bool UResearchSubsystem::HasAllRequiredWithQueue(UStaticResearch *Research) const {
  for (auto r2 : Research->RequiredResearch) {
    if (!CompletedResearches.Contains(r2) && !ResearchQueue.Contains(r2))
      return false;
  }
  return true;
}

TArray<UStaticResearch *> UResearchSubsystem::GetAllResearches() const {
  auto* mls = GEngine->GetEngineSubsystem<UModLoadingSubsystem>();
  check(mls && mls->JsonObjectLibrary);
  return mls->JsonObjectLibrary->GetObjects<UStaticResearch>();
}

const TArray<UStaticResearch *> UResearchSubsystem::GetAllResearchesSorted() const {
  auto res = GetAllResearches();

  res.Sort(
    [](const UStaticResearch &a, const UStaticResearch &b) {
      if (a.Type != b.Type) {
        return a.Type < b.Type;
      }
      if (a.MainResearch != b.MainResearch) {
        return a.MainResearch > b.MainResearch;
      }
      return a.Complexity < b.Complexity;
    });

  return res;
}

TArray<UStaticResearch *> UResearchSubsystem::GetAllResearchesSortedFiltered(const FString &Filter) const {
  auto res = GetAllResearchesSorted();

  auto filter2 = Filter.ToLower().Replace(TEXT(" "), TEXT(""));

  if (filter2.IsEmpty())
    return res;

  res = res.FilterByPredicate([&filter2](const UStaticResearch *a) { return a->SearchMetadata.Contains(filter2); });

  return res;
}

void UResearchSubsystem::TickResearch(float DeltaSeconds) {
  if (ActiveResearch) {
    ScienceSampleTimer -= DeltaSeconds;
    if (ScienceSampleTimer <= 0) {
      ScienceSampleTimer = 2;
      ScienceMedian = ScienceLast - ActiveResearchLeft;
      ScienceLast = ActiveResearchLeft;
    }

    if (ActiveResearchLeft == 0) {
      CompleteResearch(ActiveResearch);
    }
  }
}

void UResearchSubsystem::InitializeRecipeLocking() {
  // Set mDefaultLocked = true for all recipes that are unlocked by research
  for (auto research : GetAllResearches()) {
    if (auto item_r = Cast<UStaticResearchRecipe>(research))
      for (auto lock : item_r->RecipeUnlocks) {
        lock->mDefaultLocked = true;
      }
  }

  // Also set mDefaultLocked for recipes that have mUnlocksBy set but might not be in RecipeUnlocks
  for (TObjectIterator<URecipe> recipe_it; recipe_it; ++recipe_it) {
    if (recipe_it->IsTemplate()) {
      continue;
    }
    auto recipe = *recipe_it;
    if (recipe->mUnlocksBy != nullptr) {
      recipe->mDefaultLocked = true;
    }
  }

  // Reset locked state based on mDefaultLocked
  for (TObjectIterator<URecipeDictionary> recipe_d; recipe_d; ++recipe_d) {
    if (recipe_d->IsTemplate()) {
      continue;
    }
    (*recipe_d)->ResetLocked();
  }
}

void UResearchSubsystem::Reset() {
  ActiveResearch = nullptr;
  ActiveResearchLeft = 0;
  ScienceLast = 0;
  ScienceMedian = 0;
  ScienceSampleTimer = 2;
  ResearchQueue.Empty();
  CompletedResearches.Empty();
  OldResearches.Empty();
  UnlockedItems.Empty();
}

void UResearchSubsystem::InitializeResearchTreeOnStart(const UGameSessionData *gameSessionData) {
  check(gameSessionData);
  LOG(INFO_LL) << "UResearchSubsystem::InitializeResearchTreeOnStart";
  auto bAllResearchesFinishedFlag = gameSessionData->AllResearchCompleted;

  Reset();

  for (auto research : GetAllResearches()) {
    research->Type = EResearchStatus::Closed;
    research->mLevel = 0;
  }

  InitializeRecipeLocking();

  if (!bAllResearchesFinishedFlag) {
    static const FName DefaultCompleted[] = {
      TEXT("MineralsScan"), TEXT("BasicPlatform"), TEXT("Chest"), TEXT("Electricity"), TEXT("PowerGeneration"), TEXT("Smelting"), TEXT("Metalwork"), TEXT("DistributedComputing"), TEXT("CopperWire")
    };
    for (FName name : DefaultCompleted) {
      if (UStaticResearch *res = QrTryFind<UStaticResearch>(name)) {
        CompleteResearch_Internal(res);
      }
    }
  } else {
    for (auto res : GetAllResearches()) {
      CompleteResearch_Internal(res);
    }
  }

  // Open researches that have no prerequisites or all prerequisites are met
  for (auto res : GetAllResearches()) {
    if (res->Type != EResearchStatus::Complete && HasAllRequired(res)) {
      res->Type = EResearchStatus::Opened;
    }
  }

  // Update NeiComponent unlocked tab after research tree initialization
  auto controller = Cast<AMainPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
  if (controller && controller->mNei) {
    controller->mNei->UpdateUnlockedTab();
  }
}
