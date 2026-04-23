// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#include "Public/RailwayManager.h"
#include "Public/BlockLogic.h"
#include "Public/Dimension.h"
#include "Public/Inventory.h"
#include "Public/RailNetwork.h"
#include "Public/RailNodeBlockLogic.h"
#include "Public/RailTypes.h"
#include "Public/RailStationBlockLogic.h"
#include "Public/TrainSchedule.h"
#include "Public/TrainActor.h"
#include "Public/RailSplineRenderManagerActor.h"
#include "Public/Condition.h"
#include "Public/LogicContext.h"
#include "Evospace/JsonHelper.h"
#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "HAL/IConsoleManager.h"
#include "Qr/StaticLogger.h"
#include <tuple>

namespace {

uint64 IntSqrtFloor(uint64 Value) {
  uint64 low = 0;
  uint64 high = Value;
  uint64 answer = 0;
  while (low <= high) {
    const uint64 mid = low + (high - low) / 2;
    if (mid == 0 || mid <= Value / mid) {
      answer = mid;
      low = mid + 1;
    } else {
      high = mid - 1;
    }
  }
  return answer;
}

} // namespace

void URailwayManager::Initialize(ADimension *Owner, URailNetwork *Network) {
  ownerDimension = Owner;
  railNetwork = Network;
  bRailSplineVisualDirty = true;
}

void URailwayManager::BeginDestroy() {
  ReleaseRailSplineRenderer();
  Super::BeginDestroy();
}

bool URailwayManager::SerializeJson(TSharedPtr<FJsonObject> Json) const {
  if (!Json.IsValid()) {
    return false;
  }

  TArray<TSharedPtr<FJsonValue>> TrainArray;
  for (const UTrainInstance *Train : Trains) {
    if (!Train || !Train->Schedule) {
      continue;
    }

    TSharedPtr<FJsonObject> TrainJson = MakeShareable(new FJsonObject());
    TrainJson->SetNumberField(TEXT("CurrentStopIndex"), Train->CurrentStopIndex);
    TrainJson->SetStringField(TEXT("CurrentStationIdentifier"), Train->CurrentStationIdentifier);
    TrainJson->SetStringField(TEXT("TargetStationIdentifier"), Train->TargetStationIdentifier);
    TrainJson->SetNumberField(TEXT("PathIndex"), Train->PathIndex);
    TrainJson->SetNumberField(TEXT("SimState"), static_cast<int32>(Train->SimState));
    TrainJson->SetNumberField(TEXT("SimTick"), Train->SimTick);
    TrainJson->SetNumberField(TEXT("InstanceRandom"), Train->InstanceRandom);
    TrainJson->SetNumberField(TEXT("Speed"), Train->Speed);
    TrainJson->SetNumberField(TEXT("DistanceAlongSegment"), Train->DistanceAlongSegment);

    if (Train->Cargo) {
      TSharedPtr<FJsonObject> CargoJson = MakeShareable(new FJsonObject());
      if (Train->Cargo->SerializeJson(CargoJson)) {
        TrainJson->SetObjectField(TEXT("Cargo"), CargoJson);
      }
    }

    TSharedPtr<FJsonObject> ScheduleJson = MakeShareable(new FJsonObject());
    ScheduleJson->SetBoolField(TEXT("Loop"), Train->Schedule->bLoop);
    TArray<TSharedPtr<FJsonValue>> StopsJson;
    for (UTrainScheduleStop *Stop : Train->Schedule->Stops) {
      if (!Stop) {
        continue;
      }
      TSharedPtr<FJsonObject> StopJson = MakeShareable(new FJsonObject());
      StopJson->SetStringField(TEXT("StationIdentifier"), Stop->StationIdentifier);
      if (Stop->DepartureCondition) {
        json_helper::TrySerialize(StopJson, TEXT("DepartureCondition"), Stop->DepartureCondition);
      }
      StopsJson.Add(MakeShareable(new FJsonValueObject(StopJson)));
    }
    ScheduleJson->SetArrayField(TEXT("Stops"), StopsJson);
    TrainJson->SetObjectField(TEXT("Schedule"), ScheduleJson);

    TArray<TSharedPtr<FJsonValue>> PathArray;
    for (const FRailPathStep &Step : Train->Path) {
      TSharedPtr<FJsonObject> StepJson = MakeShareable(new FJsonObject());
      json_helper::TrySet(StepJson, TEXT("From"), Step.From);
      json_helper::TrySet(StepJson, TEXT("To"), Step.To);
      PathArray.Add(MakeShareable(new FJsonValueObject(StepJson)));
    }
    if (PathArray.Num() > 0) {
      TrainJson->SetArrayField(TEXT("Path"), PathArray);
    }

    TrainArray.Add(MakeShareable(new FJsonValueObject(TrainJson)));
  }

  Json->SetArrayField(TEXT("Trains"), TrainArray);
  return true;
}

bool URailwayManager::DeserializeJson(TSharedPtr<FJsonObject> Json) {
  if (!Json.IsValid()) {
    return false;
  }

  ClearAllTrains();

  const TArray<TSharedPtr<FJsonValue>> *TrainsJson = nullptr;
  if (!Json->TryGetArrayField(TEXT("Trains"), TrainsJson) || !TrainsJson) {
    return true;
  }

  for (const TSharedPtr<FJsonValue> &TrainValue : *TrainsJson) {
    TSharedPtr<FJsonObject> TrainJson = TrainValue.IsValid() ? TrainValue->AsObject() : nullptr;
    if (!TrainJson.IsValid()) {
      continue;
    }

    TSharedPtr<FJsonObject> ScheduleJson;
    if (!json_helper::TryGet(TrainJson, TEXT("Schedule"), ScheduleJson) || !ScheduleJson.IsValid()) {
      continue;
    }

    UTrainSchedule *Schedule = NewObject<UTrainSchedule>(this);
    if (!Schedule) {
      continue;
    }
    bool bLoop = true;
    if (ScheduleJson->TryGetBoolField(TEXT("Loop"), bLoop)) {
      Schedule->bLoop = bLoop;
    }

    const TArray<TSharedPtr<FJsonValue>> *StopsArray = nullptr;
    if (!ScheduleJson->TryGetArrayField(TEXT("Stops"), StopsArray) || !StopsArray || StopsArray->Num() == 0) {
      continue;
    }

    for (const TSharedPtr<FJsonValue> &StopValue : *StopsArray) {
      TSharedPtr<FJsonObject> StopJson = StopValue.IsValid() ? StopValue->AsObject() : nullptr;
      if (!StopJson.IsValid()) {
        continue;
      }

      UTrainScheduleStop *Stop = NewObject<UTrainScheduleStop>(Schedule);
      if (!Stop) {
        continue;
      }

      StopJson->TryGetStringField(TEXT("StationIdentifier"), Stop->StationIdentifier);
      if (!FindStationByIdentifier(Stop->StationIdentifier)) {
        LOG(WARN_LL) << "RailLoad: train stop station not found identifier=" << Stop->StationIdentifier;
      }

      TSharedPtr<FJsonObject> ConditionJson;
      if (json_helper::TryGet(StopJson, TEXT("DepartureCondition"), ConditionJson) && ConditionJson.IsValid()) {
        Stop->DepartureCondition = NewObject<UCondition>(Stop);
        if (Stop->DepartureCondition) {
          Stop->DepartureCondition->DeserializeJson(ConditionJson);
        }
      } else {
        Stop->DepartureCondition = NewObject<UCondition>(Stop);
        if (Stop->DepartureCondition) {
          Stop->DepartureCondition->Mode = EConditionMode::Always;
        }
      }

      Schedule->Stops.Add(Stop);
    }

    if (Schedule->Stops.Num() == 0) {
      continue;
    }

    UTrainInstance *Train = NewObject<UTrainInstance>(this);
    if (!Train) {
      continue;
    }

    Train->Schedule = Schedule;
    Train->Cargo = NewObject<UInventory>(this);
    if (Train->Cargo) {
      Train->Cargo->Resize(10);
      json_helper::TryDeserialize(TrainJson, TEXT("Cargo"), Train->Cargo);
    }
    Train->DepartureContext = NewObject<ULogicContext>(this);
    if (Train->DepartureContext && Train->DepartureContext->Input) {
      Train->DepartureContext->Input->Clear();
    }

    int32 SavedCurrentStopIndex = 0;
    TrainJson->TryGetNumberField(TEXT("CurrentStopIndex"), SavedCurrentStopIndex);
    Train->CurrentStopIndex = Schedule->Stops.IsValidIndex(SavedCurrentStopIndex) ? SavedCurrentStopIndex : 0;

    TrainJson->TryGetStringField(TEXT("CurrentStationIdentifier"), Train->CurrentStationIdentifier);
    TrainJson->TryGetStringField(TEXT("TargetStationIdentifier"), Train->TargetStationIdentifier);
    TrainJson->TryGetNumberField(TEXT("PathIndex"), Train->PathIndex);
    TrainJson->TryGetNumberField(TEXT("InstanceRandom"), Train->InstanceRandom);
    json_helper::TryGet(TrainJson, TEXT("SimTick"), Train->SimTick);
    double SavedSpeedRaw = 0.0;
    if (TrainJson->TryGetNumberField(TEXT("Speed"), SavedSpeedRaw)) {
      const bool bLegacyFloatSpeed = FMath::Abs(SavedSpeedRaw) < 10000.0;
      const double Scaled = bLegacyFloatSpeed ? SavedSpeedRaw * static_cast<double>(RailDistanceFixedScale) : SavedSpeedRaw;
      Train->Speed = FMath::Max<int64>(0, FMath::RoundToInt64(Scaled));
    } else {
      Train->Speed = 0;
    }

    double SavedDistanceRaw = 0.0;
    if (TrainJson->TryGetNumberField(TEXT("DistanceAlongSegment"), SavedDistanceRaw)) {
      const bool bLegacyFloatDistance = FMath::Abs(SavedDistanceRaw) < 100000.0;
      const double Scaled = bLegacyFloatDistance ? SavedDistanceRaw * static_cast<double>(RailDistanceFixedScale) : SavedDistanceRaw;
      Train->DistanceAlongSegment = FMath::Max<int64>(0, FMath::RoundToInt64(Scaled));
    } else {
      Train->DistanceAlongSegment = 0;
    }

    int32 SavedState = static_cast<int32>(ETrainSimState::Idle);
    TrainJson->TryGetNumberField(TEXT("SimState"), SavedState);
    Train->SimState = static_cast<ETrainSimState>(SavedState);

    const TArray<TSharedPtr<FJsonValue>> *PathArray = nullptr;
    if (TrainJson->TryGetArrayField(TEXT("Path"), PathArray) && PathArray) {
      for (const TSharedPtr<FJsonValue> &StepValue : *PathArray) {
        TSharedPtr<FJsonObject> StepJson = StepValue.IsValid() ? StepValue->AsObject() : nullptr;
        if (!StepJson.IsValid()) {
          continue;
        }
        FRailPathStep Step;
        json_helper::TryGet(StepJson, TEXT("From"), Step.From);
        json_helper::TryGet(StepJson, TEXT("To"), Step.To);
        Train->Path.Add(Step);
      }
    }

    if (!Train->CurrentStationIdentifier.IsEmpty()) {
      Train->SourceStation = FindStationByIdentifier(Train->CurrentStationIdentifier);
    }
    if (!Train->TargetStationIdentifier.IsEmpty()) {
      Train->TargetStation = FindStationByIdentifier(Train->TargetStationIdentifier);
    }

    if (Train->SimState == ETrainSimState::Moving && Train->Path.Num() == 0) {
      LOG(WARN_LL) << "RailLoad: moving train has empty path, forcing Arrived state";
      Train->SimState = ETrainSimState::Arrived;
      Train->TargetStationIdentifier.Empty();
      Train->TargetStation = nullptr;
      Train->PathIndex = 0;
      Train->DistanceAlongSegment = 0;
    }

    if (Train->PathIndex < 0 || Train->PathIndex >= Train->Path.Num()) {
      Train->PathIndex = 0;
      if (Train->SimState == ETrainSimState::Moving && Train->Path.Num() > 0) {
        Train->DistanceAlongSegment = 0;
      }
    }

    Trains.Add(Train);
    TrainVisuals.Add(nullptr);
    EnsureVisual(Trains.Num() - 1);
  }

  RefreshStationDocking();
  return true;
}

void URailwayManager::RegisterStation(URailStationBlockLogic *Station) {
  if (!Station) {
    return;
  }
  if (Station->StationID.IsEmpty()) {
    Station->StationID = GenerateStationID();
  }
  FString UniqueStationId = Station->StationID;
  int32 CollisionIndex = 1;
  while (URailStationBlockLogic *const *Found = Stations.Find(UniqueStationId)) {
    if (*Found == Station) {
      break;
    }
    UniqueStationId = FString::Printf(TEXT("%s_%03d"), *Station->StationID, CollisionIndex);
    ++CollisionIndex;
  }
  Station->StationID = UniqueStationId;
  Stations.Add(Station->StationID, Station);
}

void URailwayManager::OnRailGraphChanged() {
  bRailSplineVisualDirty = true;
  RefreshRailSplineVisuals();
}

void URailwayManager::UnregisterStation(URailStationBlockLogic *Station) {
  if (!Station) {
    return;
  }
  if (Station->StationID.IsEmpty()) {
    return;
  }
  if (URailStationBlockLogic *const *FoundStation = Stations.Find(Station->StationID)) {
    if (*FoundStation == Station) {
      Stations.Remove(Station->StationID);
    }
  }
}

URailStationBlockLogic *URailwayManager::FindStationByID(const FString &ID) const {
  if (ID.IsEmpty()) {
    return nullptr;
  }
  if (URailStationBlockLogic *const *FoundStation = Stations.Find(ID)) {
    return *FoundStation;
  }
  return nullptr;
}

FString URailwayManager::GetStationIdentifier(const URailStationBlockLogic *Station) const {
  if (!Station) {
    return FString();
  }
  return Station->StationID;
}

URailStationBlockLogic *URailwayManager::FindStationByIdentifier(const FString &StationIdentifier) const {
  return FindStationByID(StationIdentifier);
}

FQrVector3i URailwayManager::StationRootKey(URailStationBlockLogic *S) const {
  if (!S) {
    return FQrVector3i::Zero();
  }
  UBlockLogic *Base = static_cast<UBlockLogic *>(S);
  if (UBlockLogic *P = Base->GetPartRootBlock()) {
    return P->GetBlockPos();
  }
  return Base->GetBlockPos();
}

bool URailwayManager::LaunchTrain(URailStationBlockLogic *From, const FString &TargetStationIdentifier) {
  if (!From || TargetStationIdentifier.IsEmpty()) {
    return false;
  }
  UTrainSchedule *Schedule = NewObject<UTrainSchedule>(this);
  if (!Schedule) {
    return false;
  }
  Schedule->AddStopAlways(GetStationIdentifier(From));
  Schedule->AddStopAlways(TargetStationIdentifier);
  return SpawnTrainWithSchedule(Schedule, 0);
}

bool URailwayManager::SpawnTrainWithSchedule(UTrainSchedule *Schedule, int32 InitialStopIndex) {
  if (!Schedule || Schedule->Stops.Num() == 0) {
    return false;
  }
  if (!Schedule->Stops.IsValidIndex(InitialStopIndex)) {
    return false;
  }
  UTrainScheduleStop *InitialStop = Schedule->Stops[InitialStopIndex];
  if (!InitialStop) {
    return false;
  }
  const FString InitialStationIdentifier = InitialStop->StationIdentifier;
  if (!FindStationByIdentifier(InitialStationIdentifier)) {
    LOG(ERROR_LL) << "RailSpawn: initial stop station not found identifier=" << InitialStationIdentifier;
    return false;
  }

  UTrainInstance *Train = NewObject<UTrainInstance>(this);
  if (!Train) {
    return false;
  }
  Train->Schedule = Schedule;
  Train->CurrentStopIndex = InitialStopIndex;
  Train->CurrentStationIdentifier = InitialStationIdentifier;
  Train->SimState = ETrainSimState::Arrived;
  Train->Speed = 0;
  Train->InstanceRandom = FMath::Rand();
  Train->Cargo = NewObject<UInventory>(this);
  Train->DepartureContext = NewObject<ULogicContext>(this);
  if (Train->Cargo) {
    Train->Cargo->Resize(10);
  }
  if (Train->DepartureContext && Train->DepartureContext->Input) {
    Train->DepartureContext->Input->Clear();
  }
  Trains.Add(Train);
  TrainVisuals.Add(nullptr);
  EnsureVisual(Trains.Num() - 1);
  return true;
}

UTrainInstance *URailwayManager::GetTrainDataMutable(int32 Index) {
  if (!Trains.IsValidIndex(Index)) {
    return nullptr;
  }
  return Trains[Index];
}

const UTrainInstance *URailwayManager::GetTrainData(int32 Index) const {
  if (!Trains.IsValidIndex(Index)) {
    return nullptr;
  }
  return Trains[Index];
}

UTrainInstance *URailwayManager::GetDockedTrainAtStation(const FString &StationIdentifier) const {
  if (StationIdentifier.IsEmpty()) {
    return nullptr;
  }
  for (UTrainInstance *Train : Trains) {
    if (!Train) {
      continue;
    }
    if (Train->SimState == ETrainSimState::Arrived && Train->CurrentStationIdentifier == StationIdentifier) {
      return Train;
    }
  }
  return nullptr;
}

void URailwayManager::ClearAllTrains() {
  for (int32 i = 0; i < TrainVisuals.Num(); ++i) {
    ReleaseVisual(i);
  }
  TrainVisuals.Reset();
  Trains.Reset();
  RefreshStationDocking();
}

#if WITH_EDITOR
void URailwayManager::EditorDeleteAllTrains() {
  ClearAllTrains();
}
#endif

bool URailwayManager::BuildPathBetweenStations(const FString &SourceStationIdentifier, const FString &TargetStationIdentifier, TArray<FRailPathStep> &OutPath) const {
  OutPath.Empty();
  if (!railNetwork.IsValid() || SourceStationIdentifier.IsEmpty() || TargetStationIdentifier.IsEmpty() || SourceStationIdentifier == TargetStationIdentifier) {
    return false;
  }
  URailStationBlockLogic *From = FindStationByIdentifier(SourceStationIdentifier);
  URailStationBlockLogic *To = FindStationByIdentifier(TargetStationIdentifier);
  if (!From || !To) {
    return false;
  }
  const FQrVector3i SourceKey = StationRootKey(From);
  const FQrVector3i TargetKey = StationRootKey(To);
  TArray<FQrVector3i> keyPath;
  if (!railNetwork->FindPathBlockToBlock(SourceKey, TargetKey, keyPath)) {
    LOG(ERROR_LL) << "RailSchedule: path not found " << SourceKey.ToString() << " -> " << TargetKey.ToString() << " links=" << railNetwork->GetUndirectedLinkCount();
    return false;
  }
  railNetwork->BuildPathStepsFromKeyPath(keyPath, OutPath);
  return OutPath.Num() > 0;
}

bool URailwayManager::IsDepartureConditionMet(const UTrainInstance *Train) const {
  if (!Train || !Train->Schedule || !Train->Schedule->Stops.IsValidIndex(Train->CurrentStopIndex)) {
    return false;
  }
  UTrainScheduleStop *CurrentStop = Train->Schedule->Stops[Train->CurrentStopIndex];
  if (!CurrentStop) {
    return false;
  }
  URailStationBlockLogic *Station = FindStationByIdentifier(Train->CurrentStationIdentifier);
  if (!Station) {
    return false;
  }
  UCondition *Condition = CurrentStop->DepartureCondition;
  if (!Condition) {
    return true;
  }
  ULogicContext *Context = Train->GetContext_Implementation();
  if (!Context || !Context->Input) {
    return false;
  }
  return Condition->Evaluate(Context) != 0;
}

bool URailwayManager::TryDispatchTrainFromSchedule(int32 Index) {
  if (!Trains.IsValidIndex(Index)) {
    return false;
  }
  UTrainInstance *T = Trains[Index];
  if (!T || T->SimState == ETrainSimState::Moving || !T->Schedule) {
    return false;
  }
  if (!T->Schedule->Stops.IsValidIndex(T->CurrentStopIndex)) {
    return false;
  }
  UTrainScheduleStop *FromStop = T->Schedule->Stops[T->CurrentStopIndex];
  if (!FromStop) {
    return false;
  }
  if (URailStationBlockLogic *Station = FindStationByIdentifier(T->CurrentStationIdentifier)) {
    if (Station->GetDockedTrain() == T) {
      Station->SyncDepartureContextForDockedTrain();
    }
  }
  if (!IsDepartureConditionMet(T)) {
    return false;
  }

  const int32 NextStopIndex = T->Schedule->ResolveNextStopIndex(T->CurrentStopIndex);
  if (!T->Schedule->Stops.IsValidIndex(NextStopIndex)) {
    return false;
  }
  UTrainScheduleStop *ToStop = T->Schedule->Stops[NextStopIndex];
  if (!ToStop) {
    return false;
  }
  const FString SourceStationIdentifier = FromStop->StationIdentifier;
  const FString TargetStationIdentifier = ToStop->StationIdentifier;
  URailStationBlockLogic *From = FindStationByIdentifier(SourceStationIdentifier);
  URailStationBlockLogic *To = FindStationByIdentifier(TargetStationIdentifier);
  if (!From || !To) {
    return false;
  }

  TArray<FRailPathStep> path;
  if (!BuildPathBetweenStations(SourceStationIdentifier, TargetStationIdentifier, path)) {
    return false;
  }
  T->Path = MoveTemp(path);
  T->PathIndex = 0;
  T->DistanceAlongSegment = 0;
  T->TargetStationIdentifier = TargetStationIdentifier;
  T->SourceStation = From;
  T->TargetStation = To;
  T->SimState = ETrainSimState::Moving;
  T->Speed = FMath::Clamp<int64>(T->Speed, 0, static_cast<int64>(TrainMaxSpeedPerTick));
  EnsureVisual(Index);
  return true;
}

void URailwayManager::ArriveAtTarget(int32 Index, int32 ArrivedStopIndex) {
  if (!Trains.IsValidIndex(Index)) {
    return;
  }
  UTrainInstance *T = Trains[Index];
  if (!T) {
    return;
  }
  URailStationBlockLogic *ArrivedStation = T->TargetStation.Get();
  if (T->Schedule && T->Schedule->Stops.IsValidIndex(ArrivedStopIndex)) {
    if (UTrainScheduleStop *ArrivedStop = T->Schedule->Stops[ArrivedStopIndex]) {
      T->CurrentStopIndex = ArrivedStopIndex;
      T->CurrentStationIdentifier = ArrivedStop->StationIdentifier;
      T->SimState = ETrainSimState::Arrived;
    } else {
      T->SimState = ETrainSimState::Idle;
    }
  } else {
    T->SimState = ETrainSimState::Idle;
  }
  T->Path.Empty();
  T->PathIndex = 0;
  T->DistanceAlongSegment = 0;
  T->Speed = 0;
  T->SourceStation = ArrivedStation;
  T->TargetStation = nullptr;
  T->TargetStationIdentifier.Empty();
  if (T->DepartureContext && T->DepartureContext->Input) {
    T->DepartureContext->Input->Clear();
  }
}

int64 URailwayManager::ComputeRemainingPathDistance(const UTrainInstance *Train) const {
  if (!Train || !railNetwork.IsValid() || Train->Path.Num() == 0 || Train->PathIndex >= Train->Path.Num()) {
    return 0;
  }

  int64 remainingDistance = 0;
  for (int32 pathIdx = Train->PathIndex; pathIdx < Train->Path.Num(); ++pathIdx) {
    const FRailPathStep &Step = Train->Path[pathIdx];
    if (Step.From == Step.To) {
      continue;
    }

    const int64 edgeLen = railNetwork->GetEdgeLength(Step.From, Step.To);
    if (edgeLen <= 0) {
      continue;
    }

    if (pathIdx == Train->PathIndex) {
      remainingDistance += FMath::Max<int64>(0, edgeLen - Train->DistanceAlongSegment);
    } else {
      remainingDistance += edgeLen;
    }
  }
  return remainingDistance;
}

bool URailwayManager::TrySampleTrainPathAtOffset(
  const UTrainInstance *Train,
  int64 OffsetFixed,
  FVector &OutWorldLocation,
  FVector &OutWorldTangent) const {
  if (!Train || !railNetwork.IsValid() || Train->Path.Num() == 0) {
    return false;
  }

  int32 SamplePathIndex = FMath::Clamp(Train->PathIndex, 0, Train->Path.Num() - 1);
  int64 SampleDistance = Train->DistanceAlongSegment + OffsetFixed;
  const int32 LastPathIndex = Train->Path.Num() - 1;
  constexpr int32 MaxTraverseIterations = 256;

  for (int32 Iteration = 0; Iteration < MaxTraverseIterations; ++Iteration) {
    if (!Train->Path.IsValidIndex(SamplePathIndex)) {
      return false;
    }

    const FRailPathStep &Step = Train->Path[SamplePathIndex];
    const int64 StepLength = railNetwork->GetEdgeLength(Step.From, Step.To);
    if (StepLength <= 0) {
      if (SamplePathIndex >= LastPathIndex) {
        OutWorldLocation = FVector::ZeroVector;
        OutWorldTangent = FVector::ForwardVector;
        return false;
      }
      ++SamplePathIndex;
      SampleDistance = 0;
      continue;
    }

    if (SampleDistance < 0) {
      if (SamplePathIndex <= 0) {
        SampleDistance = 0;
        railNetwork->SampleEdgeWorld(Step.From, Step.To, SampleDistance, OutWorldLocation, OutWorldTangent);
        return true;
      }
      --SamplePathIndex;
      const FRailPathStep &PrevStep = Train->Path[SamplePathIndex];
      const int64 PrevLength = railNetwork->GetEdgeLength(PrevStep.From, PrevStep.To);
      SampleDistance += FMath::Max<int64>(0, PrevLength);
      continue;
    }

    if (SampleDistance > StepLength) {
      if (SamplePathIndex >= LastPathIndex) {
        SampleDistance = StepLength;
        railNetwork->SampleEdgeWorld(Step.From, Step.To, SampleDistance, OutWorldLocation, OutWorldTangent);
        return true;
      }
      SampleDistance -= StepLength;
      ++SamplePathIndex;
      continue;
    }

    railNetwork->SampleEdgeWorld(Step.From, Step.To, SampleDistance, OutWorldLocation, OutWorldTangent);
    return true;
  }

  return false;
}

void URailwayManager::UpdateTrainMovement(int32 Index) {
  if (!Trains.IsValidIndex(Index) || !railNetwork.IsValid()) {
    return;
  }
  UTrainInstance *T = Trains[Index];
  if (!T) {
    return;
  }
  if (T->Path.Num() == 0) {
    T->SimState = ETrainSimState::NoPath;
    return;
  }
  const int32 ArrivedStopIndex = T->Schedule ? T->Schedule->ResolveNextStopIndex(T->CurrentStopIndex) : INDEX_NONE;
  if (T->PathIndex >= T->Path.Num()) {
    ArriveAtTarget(Index, ArrivedStopIndex);
    return;
  }
  const FRailPathStep &Step = T->Path[T->PathIndex];
  if (Step.From == Step.To) {
    T->SimState = ETrainSimState::NoPath;
    return;
  }
  const int64 len = railNetwork->GetEdgeLength(Step.From, Step.To);
  if (len <= 0) {
    T->PathIndex++;
    T->DistanceAlongSegment = 0;
    if (T->PathIndex >= T->Path.Num()) {
      ArriveAtTarget(Index, ArrivedStopIndex);
    }
    return;
  }

  const int64 remainingDistance = ComputeRemainingPathDistance(T);
  if (remainingDistance <= TrainStopDistanceTolerance) {
    ArriveAtTarget(Index, ArrivedStopIndex);
    return;
  }

  const int64 accelerationPerTick = FMath::Max<int64>(0, TrainAccelerationPerTick);
  const int64 brakingPerTick = FMath::Max<int64>(1, TrainBrakingPerTick);
  const int64 maxSpeedPerTick = FMath::Max<int64>(0, TrainMaxSpeedPerTick);

  int64 desiredSpeed = maxSpeedPerTick;
  const int64 brakingDistanceAtMaxSpeed = (maxSpeedPerTick <= 0 || brakingPerTick <= 0)
                                            ? 0
                                            : (maxSpeedPerTick * maxSpeedPerTick) / (2 * brakingPerTick);
  const int64 brakingZoneStartDistance = TrainStopDistanceTolerance + brakingDistanceAtMaxSpeed;
  if (remainingDistance <= brakingZoneStartDistance) {
    const int64 stopDistance = FMath::Max<int64>(0, remainingDistance - TrainStopDistanceTolerance);
    const uint64 stopDistanceU = static_cast<uint64>(stopDistance);
    const uint64 brakingU = static_cast<uint64>(brakingPerTick);
    const uint64 maxSafeSpeedToStop = IntSqrtFloor(2ull * brakingU * stopDistanceU);
    desiredSpeed = FMath::Min<int64>(maxSpeedPerTick, static_cast<int64>(maxSafeSpeedToStop));
  }

  if (T->Speed < desiredSpeed) {
    T->Speed = FMath::Min<int64>(desiredSpeed, T->Speed + accelerationPerTick);
  } else {
    T->Speed = FMath::Max<int64>(desiredSpeed, T->Speed - brakingPerTick);
  }
  T->Speed = FMath::Clamp<int64>(T->Speed, 0, maxSpeedPerTick);

  int64 travelDistance = T->Speed;
  while (travelDistance > 0) {
    if (T->PathIndex >= T->Path.Num()) {
      T->DistanceAlongSegment = 0;
      ArriveAtTarget(Index, ArrivedStopIndex);
      return;
    }

    const FRailPathStep &CurrentStep = T->Path[T->PathIndex];
    if (CurrentStep.From == CurrentStep.To) {
      T->SimState = ETrainSimState::NoPath;
      return;
    }

    const int64 currentLen = railNetwork->GetEdgeLength(CurrentStep.From, CurrentStep.To);
    if (currentLen <= 0) {
      ++T->PathIndex;
      T->DistanceAlongSegment = 0;
      continue;
    }

    const int64 segmentRemaining = FMath::Max<int64>(0, currentLen - T->DistanceAlongSegment);
    if (travelDistance < segmentRemaining) {
      T->DistanceAlongSegment += travelDistance;
      break;
    }

    travelDistance -= segmentRemaining;
    ++T->PathIndex;
    T->DistanceAlongSegment = 0;
    if (T->PathIndex >= T->Path.Num()) {
      ArriveAtTarget(Index, ArrivedStopIndex);
      return;
    }
  }
}

void URailwayManager::RefreshStationDocking() {
  TMap<FString, UTrainInstance *> StationIdentifierToTrain;
  for (UTrainInstance *Train : Trains) {
    if (!Train || Train->SimState != ETrainSimState::Arrived || Train->CurrentStationIdentifier.IsEmpty()) {
      continue;
    }
    StationIdentifierToTrain.FindOrAdd(Train->CurrentStationIdentifier) = Train;
  }

  for (const TPair<FString, URailStationBlockLogic *> &Pair : Stations) {
    URailStationBlockLogic *Station = Pair.Value;
    if (!Station) {
      continue;
    }
    UTrainInstance *WantTrain = StationIdentifierToTrain.FindRef(GetStationIdentifier(Station));
    if (Station->GetDockedTrain() != WantTrain) {
      Station->SetDockedTrain(WantTrain);
    }
  }
}

bool URailwayManager::TryGetTrainTransform(const UTrainInstance *T, FVector &OutWorldLocation, FRotator &OutWorldRotation) const {
  if (!T) {
    return false;
  }

  FVector CenterLocation;
  FVector CenterTangent;
  if (TryGetTrainCenterPose(T, CenterLocation, CenterTangent)) {
    OutWorldLocation = CenterLocation;
    const FVector Forward = CenterTangent.GetSafeNormal();
    OutWorldRotation = Forward.IsNearlyZero() ? FRotator::ZeroRotator : Forward.Rotation();
    return true;
  }

  URailStationBlockLogic *Station = FindStationByIdentifier(T->CurrentStationIdentifier);
  if (!Station) {
    return false;
  }

  UBlockLogic *StationRoot = Station->GetPartRootBlock();
  if (!StationRoot) {
    StationRoot = Station;
  }
  OutWorldLocation = StationRoot->GetLocation() + FVector(0.f, 0.f, TrainBogieZOffsetUu);
  OutWorldRotation = StationRoot->GetBlockQuat().Rotator();
  return true;
}

bool URailwayManager::TryGetTrainCenterPose(
  const UTrainInstance *Train,
  FVector &OutCenterLocation,
  FVector &OutCenterTangent) const {
  if (!Train) {
    return false;
  }

  if (TrySampleTrainPathAtOffset(Train, 0, OutCenterLocation, OutCenterTangent)) {
    OutCenterLocation += FVector(0.0f, 0.0f, TrainBogieZOffsetUu);
    return true;
  }

  URailStationBlockLogic *Station = FindStationByIdentifier(Train->CurrentStationIdentifier);
  if (!Station) {
    return false;
  }
  UBlockLogic *StationRoot = Station->GetPartRootBlock();
  if (!StationRoot) {
    StationRoot = Station;
  }
  OutCenterLocation = StationRoot->GetLocation() + FVector(0.0f, 0.0f, TrainBogieZOffsetUu);
  const FVector Forward = StationRoot->GetBlockQuat().GetForwardVector().GetSafeNormal();
  OutCenterTangent = Forward.IsNearlyZero() ? FVector::ForwardVector : Forward;
  return true;
}

bool URailwayManager::TryGetTrainVisualPose(
  const UTrainInstance *Train,
  FVector &OutFrontBogieLocation,
  FVector &OutFrontBogieTangent,
  FVector &OutRearBogieLocation,
  FVector &OutRearBogieTangent) const {
  if (!Train) {
    return false;
  }

  const float HalfSpacingUu = FMath::Max(1.0f, TrainBogieSpacingUu * 0.5f);
  const int64 HalfSpacingFixed = FMath::RoundToInt64(static_cast<double>(HalfSpacingUu) * static_cast<double>(RailDistanceFixedScale));
  FVector CenterLocation;
  FVector CenterTangent;
  if (!TryGetTrainCenterPose(Train, CenterLocation, CenterTangent)) {
    return false;
  }

  FVector BogieAxis = CenterTangent.GetSafeNormal();
  if (BogieAxis.IsNearlyZero()) {
    BogieAxis = FVector::ForwardVector;
  }

  FVector SampledFrontLocation;
  FVector SampledFrontTangent;
  FVector SampledRearLocation;
  FVector SampledRearTangent;
  const bool bHasRailSamples = TrySampleTrainPathAtOffset(Train, HalfSpacingFixed, SampledFrontLocation, SampledFrontTangent) &&
                               TrySampleTrainPathAtOffset(Train, -HalfSpacingFixed, SampledRearLocation, SampledRearTangent);
  if (bHasRailSamples) {
    const FVector VerticalOffset(0.0f, 0.0f, TrainBogieZOffsetUu);
    SampledFrontLocation += VerticalOffset;
    SampledRearLocation += VerticalOffset;

    const FVector SampledAxis = (SampledFrontLocation - SampledRearLocation).GetSafeNormal();
    if (!SampledAxis.IsNearlyZero()) {
      BogieAxis = SampledAxis;
    }

    OutFrontBogieTangent = SampledFrontTangent.GetSafeNormal();
    OutRearBogieTangent = SampledRearTangent.GetSafeNormal();
  } else {
    OutFrontBogieTangent = BogieAxis;
    OutRearBogieTangent = BogieAxis;
  }

  if (OutFrontBogieTangent.IsNearlyZero()) {
    OutFrontBogieTangent = BogieAxis;
  }
  if (OutRearBogieTangent.IsNearlyZero()) {
    OutRearBogieTangent = BogieAxis;
  }

  OutFrontBogieLocation = CenterLocation + BogieAxis * HalfSpacingUu;
  OutRearBogieLocation = CenterLocation - BogieAxis * HalfSpacingUu;
  return true;
}

void URailwayManager::Tick(float SimStepSeconds) {
  (void)SimStepSeconds;
  if (!ownerDimension.IsValid() || !railNetwork.IsValid()) {
    return;
  }
  // Before dispatch: stations must have DockedTrain / WagonStorageProxy in sync with simulation (Arrived + CurrentStationIdentifier).
  RefreshStationDocking();
  for (int i = 0; i < Trains.Num(); ++i) {
    UTrainInstance *Train = Trains[i];
    if (!Train) {
      continue;
    }
    ++Train->SimTick;
    if (Train->SimState == ETrainSimState::Moving) {
      UpdateTrainMovement(i);
      continue;
    }
    std::ignore = TryDispatchTrainFromSchedule(i);
  }
}

void URailwayManager::EnsureVisual(int32 Index) {
  if (!Trains.IsValidIndex(Index)) {
    return;
  }
  if (TrainVisuals.Num() < Trains.Num()) {
    TrainVisuals.SetNum(Trains.Num());
  }
  if (!TrainVisuals.IsValidIndex(Index)) {
    return;
  }
  ADimension *D = ownerDimension.Get();
  if (!D || !D->GetWorld()) {
    return;
  }
  if (TrainVisuals[Index]) {
    return;
  }
  const UTrainInstance *TD = Trains[Index];
  FVector at;
  FRotator Rot;
  if (!TryGetTrainTransform(TD, at, Rot)) {
    return;
  }
  FActorSpawnParameters sp;
  sp.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
  sp.Owner = D;
  ATrainActor *a = D->GetWorld()->SpawnActor<ATrainActor>(at, Rot, sp);
  if (a) {
    a->BindToTrain(this, Index);
    TrainVisuals[Index] = a;
  }
}

void URailwayManager::ReleaseVisual(int32 Index) {
  if (!TrainVisuals.IsValidIndex(Index)) {
    return;
  }
  if (TrainVisuals[Index]) {
    TrainVisuals[Index]->Destroy();
  }
  TrainVisuals[Index] = nullptr;
}

void URailwayManager::TickVisual(float DeltaTime) {
  (void)DeltaTime;
  if (!ownerDimension.IsValid() || !railNetwork.IsValid()) {
    return;
  }
  RefreshRailSplineVisuals();
  for (int i = 0; i < Trains.Num(); ++i) {
    UTrainInstance *T = Trains[i];
    if (!T) {
      continue;
    }
    EnsureVisual(i);
    if (ATrainActor *Vis = TrainVisuals[i]) {
      Vis->BindToTrain(this, i);
      FVector CenterLocation;
      FVector CenterTangent;
      FVector FrontBogieLocation;
      FVector FrontBogieTangent;
      FVector RearBogieLocation;
      FVector RearBogieTangent;
      if (TryGetTrainCenterPose(T, CenterLocation, CenterTangent) &&
          TryGetTrainVisualPose(T, FrontBogieLocation, FrontBogieTangent, RearBogieLocation, RearBogieTangent)) {
        Vis->ApplyRailPose(CenterLocation, CenterTangent, FrontBogieLocation, FrontBogieTangent, RearBogieLocation, RearBogieTangent);
      } else {
        FVector at;
        FRotator r;
        if (TryGetTrainTransform(T, at, r)) {
          Vis->SetActorLocationAndRotation(at, r);
        }
      }
    }
  }
  DebugDrawRailGraph();
}

void URailwayManager::EnsureRailSplineRenderer() {
  if (RailSplineRenderer.IsValid()) {
    return;
  }
  ADimension *Dimension = ownerDimension.Get();
  if (!Dimension) {
    return;
  }
  UWorld *World = Dimension->GetWorld();
  if (!World) {
    return;
  }

  FActorSpawnParameters SpawnParams;
  SpawnParams.Owner = Dimension;
  SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
  SpawnParams.ObjectFlags |= RF_Transient;

  ARailSplineRenderManagerActor *SpawnedActor = World->SpawnActor<ARailSplineRenderManagerActor>(FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
  if (!SpawnedActor) {
    return;
  }
  SpawnedActor->AttachToActor(Dimension, FAttachmentTransformRules::KeepWorldTransform);
  RailSplineRenderer = SpawnedActor;
}

void URailwayManager::ReleaseRailSplineRenderer() {
  if (!RailSplineRenderer.IsValid()) {
    return;
  }
  RailSplineRenderer->Destroy();
  RailSplineRenderer = nullptr;
}

void URailwayManager::RefreshRailSplineVisuals() {
  if (!RailSplineRenderer.IsValid()) {
    bRailSplineVisualDirty = true;
  }
  if (!bRailSplineVisualDirty) {
    return;
  }
  if (!ownerDimension.IsValid() || !railNetwork.IsValid()) {
    return;
  }
  EnsureRailSplineRenderer();
  if (!RailSplineRenderer.IsValid()) {
    return;
  }

  TArray<FRailRenderSegmentData> Segments;
  railNetwork->CollectRenderSegments(Segments);
  RailSplineRenderer->RebuildRailSegments(Segments);
  bRailSplineVisualDirty = false;
}

void URailwayManager::DebugDrawRailGraph() const {
  if (!bDebugDrawRailGraph || !ownerDimension.IsValid() || !railNetwork.IsValid()) {
    return;
  }
  UWorld *W = ownerDimension->GetWorld();
  if (!W) {
    return;
  }
  const float LineLife = 0.12f;
  const FColor LineColor(0, 220, 255);
  railNetwork->DebugDrawGraphEdges(W, LineColor, LineLife);
}

FString URailwayManager::GenerateStationID() const {
  FString GeneratedId;
  do {
    GeneratedId = FString::Printf(TEXT("RailStation_%08X"), FMath::Rand());
  } while (Stations.Contains(GeneratedId));
  return GeneratedId;
}

namespace {

static ADimension *FindDimensionForRailCommand(UWorld *World) {
  if (!World) {
    return nullptr;
  }
  for (TActorIterator<ADimension> It(World); It; ++It) {
    return *It;
  }
  return nullptr;
}

static bool ParseStationIdArg(const TArray<FString> &Args, int32 ArgIndex, FString &OutStationId) {
  if (!Args.IsValidIndex(ArgIndex)) {
    return false;
  }
  OutStationId = Args[ArgIndex];
  return !OutStationId.IsEmpty();
}

static bool BuildScheduleStopIds(const TArray<FString> &Args, const TMap<FString, URailStationBlockLogic *> &Stations, TArray<FString> &OutStopIds) {
  OutStopIds.Empty();
  if (Args.Num() == 0) {
    for (const TPair<FString, URailStationBlockLogic *> &Pair : Stations) {
      if (!Pair.Value) {
        continue;
      }
      OutStopIds.Add(Pair.Key);
      if (OutStopIds.Num() == 2) {
        return true;
      }
    }
    return OutStopIds.Num() >= 1;
  }

  for (int32 i = 0; i < Args.Num(); ++i) {
    FString StationId;
    if (!ParseStationIdArg(Args, i, StationId)) {
      return false;
    }
    OutStopIds.Add(StationId);
  }
  return OutStopIds.Num() >= 1;
}

static FAutoConsoleCommandWithWorldAndArgs RailSpawnTrainCmd(
  TEXT("Evospace.Rail.SpawnTrain"),
  TEXT("Spawn one train with schedule. Usage: Evospace.Rail.SpawnTrain [stationName ...] (one or more)"),
  FConsoleCommandWithWorldAndArgsDelegate::CreateStatic([](const TArray<FString> &Args, UWorld *World) {
    ADimension *Dimension = FindDimensionForRailCommand(World);
    if (!Dimension) {
      LOG(ERROR_LL) << "RailSpawnTrain: no ADimension found for current world";
      return;
    }

    URailwayManager *RailwayManager = Dimension->GetRailwayManager();
    if (!RailwayManager) {
      LOG(ERROR_LL) << "RailSpawnTrain: railway manager is unavailable";
      return;
    }

    const TMap<FString, URailStationBlockLogic *> &Stations = RailwayManager->GetStations();
    if (Stations.Num() < 1) {
      LOG(ERROR_LL) << "RailSpawnTrain: no registered stations, current=" << Stations.Num();
      return;
    }

    TArray<FString> StopIds;
    if (!BuildScheduleStopIds(Args, Stations, StopIds)) {
      LOG(ERROR_LL) << "RailSpawnTrain: usage Evospace.Rail.SpawnTrain [stationName1 stationName2 ...]";
      return;
    }

    UTrainSchedule *Schedule = NewObject<UTrainSchedule>(RailwayManager);
    if (!Schedule) {
      LOG(ERROR_LL) << "RailSpawnTrain: failed to create schedule";
      return;
    }
    for (const FString &StationId : StopIds) {
      URailStationBlockLogic *Station = RailwayManager->FindStationByID(StationId);
      if (!Station) {
        LOG(ERROR_LL) << "RailSpawnTrain: station not found id=" << StationId;
        return;
      }
      Schedule->AddStopAlways(RailwayManager->GetStationIdentifier(Station));
    }
    if (Schedule->Stops.Num() == 0) {
      LOG(ERROR_LL) << "RailSpawnTrain: schedule has no stops";
      return;
    }
    if (!RailwayManager->SpawnTrainWithSchedule(Schedule, 0)) {
      LOG(ERROR_LL) << "RailSpawnTrain: failed to spawn train with schedule";
      return;
    }

    LOG(INFO_LL) << "RailSpawnTrain: spawned scheduled train, stops=" << Schedule->Stops.Num();
  }));

} // namespace
