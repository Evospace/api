#include "Public/TrainSchedule.h"
#include "Public/Condition.h"

void UTrainSchedule::AddStopAlways(const FString &StationIdentifier) {
  if (StationIdentifier.IsEmpty()) {
    return;
  }
  UTrainScheduleStop *const Stop = NewObject<UTrainScheduleStop>(this);
  if (!Stop) {
    return;
  }
  Stop->StationIdentifier = StationIdentifier;
  Stop->DepartureCondition = NewObject<UCondition>(Stop);
  if (Stop->DepartureCondition) {
    Stop->DepartureCondition->Mode = EConditionMode::Always;
  }
  Stops.Add(Stop);
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
