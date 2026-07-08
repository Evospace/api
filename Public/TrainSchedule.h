#pragma once

#include "CoreMinimal.h"
#include "Qr/Prototype.h"
#include "TrainSchedule.generated.h"

class UCondition;

UCLASS(BlueprintType)
class UTrainScheduleStop : public UInstance {
  GENERATED_BODY()
  EVO_CODEGEN_INSTANCE(TrainScheduleStop)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UTrainScheduleStop, UInstance>("TrainScheduleStop") //@class TrainScheduleStop : Instance
      .endClass();
  }

  public:
  // Non-unique station name (Factorio semantics: the train goes to any reachable
  // station carrying this name).
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rail|Schedule")
  FString StationName;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rail|Schedule")
  UCondition *DepartureCondition = nullptr;
};

UCLASS(BlueprintType)
class UTrainSchedule : public UObject {
  GENERATED_BODY()

  public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rail|Schedule")
  bool bLoop = true;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rail|Schedule")
  TArray<UTrainScheduleStop *> Stops;

  void AddStopAlways(const FString &StationName);
  /** Inserts a stop with an Always departure condition; Index outside [0, Num] appends. Returns the new stop or null. */
  UTrainScheduleStop *InsertStopAlways(const FString &StationName, int32 Index);
  int32 ResolveNextStopIndex(int32 CurrentStopIndex) const;
};
