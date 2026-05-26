#pragma once

#include "CoreMinimal.h"
#include "Public/EventSystem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "QuestSubsystem.generated.h"

struct lua_State;

class UStaticChapter;
class UStaticQuest;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuestStateChanged, UStaticQuest *, Quest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChapterStateChanged, UStaticChapter *, Chapter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuestObjectivesChanged, UStaticQuest *, Quest);

UCLASS()
class UQuestSubsystem : public UGameInstanceSubsystem {
  GENERATED_BODY()

  public:
  virtual void Initialize(FSubsystemCollectionBase &Collection) override;
  virtual void Deinitialize() override;

  UFUNCTION(BlueprintCallable)
  bool UnlockChapter(UStaticChapter *Chapter);

  UFUNCTION(BlueprintCallable)
  bool UnlockQuest(UStaticQuest *Quest);

  UFUNCTION(BlueprintCallable)
  bool CompleteQuest(UStaticQuest *Quest);

  UFUNCTION(BlueprintCallable, BlueprintPure)
  bool IsQuestActive(UStaticQuest *Quest) const;

  UFUNCTION(BlueprintCallable, BlueprintPure)
  bool IsQuestCompleted(UStaticQuest *Quest) const;

  UFUNCTION(BlueprintCallable)
  TArray<UStaticQuest *> GetAllQuests() const;

  UFUNCTION(BlueprintCallable)
  TArray<UStaticChapter *> GetAllChapters() const;

  UPROPERTY(BlueprintAssignable)
  FQuestStateChanged OnQuestUnlocked;

  UPROPERTY(BlueprintAssignable)
  FQuestStateChanged OnQuestCompleted;

  UPROPERTY(BlueprintAssignable)
  FChapterStateChanged OnChapterUnlocked;

  UPROPERTY(BlueprintAssignable)
  FChapterStateChanged OnChapterCompleted;

  UPROPERTY(BlueprintAssignable)
  FQuestObjectivesChanged OnQuestObjectivesChanged;

  void NotifyQuestObjectivesChanged(UStaticQuest *Quest);

  void InitializeForNewSession();

  static void lua_reg(lua_State *L);

  private:
  UFUNCTION()
  void LoadAllDefinitions();

  UFUNCTION()
  void LoadProgress(const FString &saveName);

  UFUNCTION()
  void SaveProgress(const FString &saveName);

  bool HasRequiredQuests(UStaticQuest *Quest) const;
  bool HasRequiredChapter(UStaticChapter *Chapter) const;

  void SubscribeQuestEvents(UStaticQuest *Quest);
  void UnsubscribeQuestEvents(UStaticQuest *Quest);

  void TryAutoUnlockQuests();
  void TryAutoUnlockChapters();
  void TryCompleteChapter(UStaticChapter *Chapter);

  EventSystem::SubscriberGroup GroupForQuest(UStaticQuest *Quest) const;

  UPROPERTY()
  TArray<UStaticQuest *> AllQuests;

  UPROPERTY()
  TArray<UStaticChapter *> AllChapters;
};
