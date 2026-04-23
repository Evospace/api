#include "Public/TrainSchedule.h"
#include "Public/Condition.h"

void UTrainSchedule::AddStopAlways(int32 StationId) {
  if (StationId <= 0) {
    return;
  }
  FTrainScheduleStop stop;
  stop.StationId = StationId;
  stop.DepartureCondition = NewObject<UCondition>(this);
  if (stop.DepartureCondition) {
    stop.DepartureCondition->Mode = EConditionMode::Always;
  }
  Stops.Add(MoveTemp(stop));
}

int32 UTrainSchedule::ResolveNextStopIndex(int32 CurrentStopIndex) const {
  if (Stops.Num() <= 1) {
    return INDEX_NONE;
  }
  const int32 NextIndex = CurrentStopIndex + 1;
  if (Stops.IsValidIndex(NextIndex)) {
    return NextIndex;
  }
  if (bLoop) {
    return 0;
  }
  return INDEX_NONE;
}
