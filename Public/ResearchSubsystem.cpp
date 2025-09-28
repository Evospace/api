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
#include "Qr/AchievementSubsystem.h"
#include "Qr/Ensure.h"
#include "Public/MainGameInstance.h"

#include <Engine/Engine.h>
#include <EngineUtils.h>

void UResearchSubsystem::Initialize(FSubsystemCollectionBase &Collection) {
  Super::Initialize(Collection);
}

void UResearchSubsystem::Deinitialize() {
  Super::Deinitialize();
}

bool UResearchSubsystem::DeserializeFromPlayerJson(TSharedPtr<FJsonObject> json) {
  if (!json)
    return false;

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

  OnResearchQueueUpdated.Broadcast();
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

void UResearchSubsystem::ApplyLoadedCompletedResearches() {
  // Post-process loaded researches: apply effects and fix statuses
  for (auto r : CompletedResearches) {
    if (expect(r, "UResearchSubsystem::ApplyLoadedCompletedResearches: research is nullptr")) {
      auto controller = Cast<AMainPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
      r->ApplyToController(controller, r->mLevel + 1);
      r->Type = EResearchStatus::Complete;
    }
  }
}

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
    OnResearchQueueUpdated.Broadcast();
  }
}

bool UResearchSubsystem::CanEnqueueResearch(UStaticResearch *Research) const {
  if (!Research)
    return false;
  if (ResearchQueue.Find(Research) != INDEX_NONE)
    return false;
  if (Research->Type == EResearchStatus::Restricted)
    return false;
  if (Research->Type == EResearchStatus::Closed)
    return false;
  return true;
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

  if (Research->GetName() == TEXT("Portal")) {
    if (auto ach = GetGameInstance()->GetSubsystem<UAchievementSubsystem>()) {
      ach->IncrementRecord(TEXT("SilverGod"), 1);
    }
  }

  bool goldenGod = true;
  for (const auto res_test : GetAllResearches()) {
    if (res_test->Type != EResearchStatus::Complete) {
      goldenGod = false;
      break;
    }
  }
  if (goldenGod) {
    if (auto ach = GetGameInstance()->GetSubsystem<UAchievementSubsystem>()) {
      ach->IncrementRecord(TEXT("GoldenGod"), 1);
    }
  }

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
  return ActiveResearchLeft / static_cast<float>(ScienceMedian + 1) * 2.f;
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

TArray<UStaticResearch *> &UResearchSubsystem::GetAllResearches() const {
  if (AllResearchesCache.Num() == 0) {
    for (TObjectIterator<UStaticResearch> unlock; unlock; ++unlock) {
      if (unlock->IsTemplate()) {
        continue;
      }
      AllResearchesCache.Add(*unlock);
    }
  }
  return AllResearchesCache;
}

TArray<UStaticResearch *> UResearchSubsystem::GetAllResearchesSorted() const {
  auto res = GetAllResearches();

  res.Sort([](const UStaticResearch &a, const UStaticResearch &b) {
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

  res = res.FilterByPredicate([&filter2](const UStaticResearch *a) {
    return a->SearchMetadata.Contains(filter2);
  });

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

void UResearchSubsystem::InitializeResearchTreeOnStart() {
  auto inst = Cast<UMainGameInstance>(GetGameInstance());
  if (!inst)
    return;

  auto bAllResearchesFinishedFlag = inst->GetSubsystem<UGameSessionSubsystem>()->Data->AllResearchCompleted;

  if (!bAllResearchesFinishedFlag) {
    for (auto res : UMainGameInstance::Singleton->GetObjectLibrary()->GetObjects<UStaticResearch>()) {
      if (res->mCompleteByDefault)
        CompleteResearch_Internal(res);
    }
  } else {
    for (auto res : UMainGameInstance::Singleton->GetObjectLibrary()->GetObjects<UStaticResearch>()) {
      CompleteResearch_Internal(res);
    }
  }

  for (auto res : UMainGameInstance::Singleton->GetObjectLibrary()->GetObjectsMut<UStaticResearch>()) {
    if (res->Type != EResearchStatus::Complete) {
      for (auto req : res->RequiredResearch) {
        if (!CompletedResearches.Contains(req) && !ResearchQueue.Contains(req)) {
          res->Type = EResearchStatus::Closed;
          break;
        }
        if (ResearchQueue.Contains(req)) {
          res->Type = EResearchStatus::CanEnqueue;
        }
      }
    }
  }
}


