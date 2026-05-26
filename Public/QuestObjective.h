#pragma once

#include "Qr/Loc.h"
#include "Qr/Prototype.h"
#include "QuestObjective.generated.h"

class UStaticQuest;

UCLASS(BlueprintType)
class UQuestObjective : public UInstance {
  GENERATED_BODY()

  public:
  using Self = UQuestObjective;
  EVO_CODEGEN_INSTANCE(QuestObjective)

  virtual void lua_reg(lua_State *L) const override;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FName ObjectiveId = NAME_None;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FLoc Label;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool Completed = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool ShowProgress = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int64 Current = 0;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int64 Required = 0;

  UFUNCTION(BlueprintCallable)
  void SetProgress(int64 InCurrent, int64 InRequired, bool bShowProgress = true);

  UFUNCTION(BlueprintCallable)
  void SetCompleted(bool bCompleted);
};
