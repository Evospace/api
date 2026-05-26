#pragma once

#include "Public/EventSystem.h"
#include "Public/QuestObjective.h"
#include "Qr/Loc.h"
#include "Qr/Prototype.h"
#include "StaticQuest.generated.h"

class UStaticChapter;

UENUM(BlueprintType)
enum class EQuestState : uint8 {
  Locked,
  Active,
  Completed
};

UCLASS(BlueprintType)
class UStaticQuest : public UPrototype {
  GENERATED_BODY()

  public:
  using Self = UStaticQuest;
  PROTOTYPE_CODEGEN(StaticQuest, StaticQuest)

  virtual void lua_reg(lua_State *L) const override;
  virtual UClass *GetSuperProto() const override { return StaticClass(); }
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual void ModLoadFinalize() override;
  virtual void Release() override;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  FLoc Label;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  TArray<FLoc> DescriptionParts;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  UStaticChapter *Chapter = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  TArray<UStaticQuest *> RequiredQuests;

  /** Auto-unlock when prerequisites are met and the parent chapter is unlocked. */
  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  bool AutoUnlock = true;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
  EQuestState State = EQuestState::Locked;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  TArray<UQuestObjective *> Objectives;

  std::optional<luabridge::LuaRef> Events;
  std::optional<luabridge::LuaRef> OnUnlock;
  std::optional<luabridge::LuaRef> OnComplete;

  TMap<defines::Event, luabridge::LuaRef> EventHandlers;

  EventSystem::SubscriberGroup SubscriptionGroup = EventSystem::NoGroup;

  void RefreshEventHandlers();
  void Complete();

  UFUNCTION(BlueprintCallable)
  void ClearObjectives();

  UFUNCTION(BlueprintCallable)
  UQuestObjective *CreateObjective(FName ObjectiveId = NAME_None);

  UFUNCTION(BlueprintCallable)
  void AddObjective(UQuestObjective *Objective);

  UFUNCTION(BlueprintCallable, BlueprintPure)
  UQuestObjective *FindObjectiveById(FName ObjectiveId) const;

  UFUNCTION(BlueprintCallable)
  bool SetObjectiveComplete(int32 Index, bool bCompleted);

  UFUNCTION(BlueprintCallable)
  bool SetObjectiveCompleteById(FName ObjectiveId, bool bCompleted);

  UFUNCTION(BlueprintCallable)
  bool SetObjectiveProgress(int32 Index, int64 Current, int64 Required, bool bShowProgress = true);

  UFUNCTION(BlueprintCallable)
  bool SetObjectiveLabel(int32 Index, const FLoc &InLabel);

  void SetObjectivesFromLua(const luabridge::LuaRef &table);

  void NotifyObjectivesChanged();

  private:
  FName PendingChapterName;
  TArray<FName> PendingRequiredQuestNames;

  void FinalizeEventHandlers();
  int32 FindObjectiveIndexById(FName ObjectiveId) const;
  int32 FindObjectiveIndex(UQuestObjective *Objective) const;
};
