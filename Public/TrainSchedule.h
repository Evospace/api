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
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rail|Schedule")
  FString StationIdentifier;

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

  void AddStopAlways(const FString &StationIdentifier);
  int32 ResolveNextStopIndex(int32 CurrentStopIndex) const;
};
