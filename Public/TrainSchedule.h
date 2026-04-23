#pragma once

#include "CoreMinimal.h"
#include "TrainSchedule.generated.h"

class UCondition;

USTRUCT(BlueprintType)
struct FTrainScheduleStop {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rail|Schedule")
  int32 StationId = 0;

  UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Rail|Schedule")
  UCondition *DepartureCondition = nullptr;
};

UCLASS(BlueprintType)
class UTrainSchedule : public UObject {
  GENERATED_BODY()

  public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rail|Schedule")
  bool bLoop = true;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rail|Schedule")
  TArray<FTrainScheduleStop> Stops;

  void AddStopAlways(int32 StationId);
  bool HasEnoughStops() const { return Stops.Num() >= 2; }
  int32 ResolveNextStopIndex(int32 CurrentStopIndex) const;
};
