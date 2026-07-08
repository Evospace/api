#include "Public/TrainSchedule.h"
#include "Public/Condition.h"

void UTrainSchedule::AddStopAlways(const FString &StationName) {
  InsertStopAlways(StationName, Stops.Num());
}

UTrainScheduleStop *UTrainSchedule::InsertStopAlways(const FString &StationName, int32 Index) {
  if (StationName.IsEmpty()) {
    return nullptr;
  }
  UTrainScheduleStop *const Stop = NewObject<UTrainScheduleStop>(this);
  if (!Stop) {
    return nullptr;
  }
  Stop->StationName = StationName;
  Stop->DepartureCondition = NewObject<UCondition>(Stop);
  if (Stop->DepartureCondition) {
    Stop->DepartureCondition->Mode = EConditionMode::Always;
  }
  if (Index < 0 || Index > Stops.Num()) {
    Index = Stops.Num();
  }
  Stops.Insert(Stop, Index);
  return Stop;
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
