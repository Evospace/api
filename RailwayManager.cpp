// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#include "Public/RailwayManager.h"
#include "Public/BlockLogic.h"
#include "Public/Dimension.h"
#include "Public/Inventory.h"
#include "Public/RailNetwork.h"
#include "Public/RailNodeBlockLogic.h"
#include "Public/RailStationBlockLogic.h"
#include "Public/TrainSchedule.h"
#include "Public/TrainActor.h"
#include "Public/Condition.h"
#include "Public/LogicContext.h"
#include "Evospace/Item/InventoryLibrary.h"
#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "HAL/IConsoleManager.h"
#include "Qr/StaticLogger.h"
#include <tuple>

void URailwayManager::Initialize(ADimension *Owner, URailNetwork *Network) {
  ownerDimension = Owner;
  railNetwork = Network;
}

void URailwayManager::RegisterStation(URailStationBlockLogic *Station) {
  if (!Station) {
    return;
  }
  if (Station->StationID == 0) {
    Station->StationID = GenerateStationID();
  } else {
    NextStationId = FMath::Max(NextStationId, Station->StationID + 1);
  }
  Stations.Add(Station->StationID, Station);
}

void URailwayManager::OnRailGraphChanged() {
}

void URailwayManager::UnregisterStation(URailStationBlockLogic *Station) {
  if (!Station) {
    return;
  }
  if (Station->StationID == 0) {
    return;
  }
  if (URailStationBlockLogic *const *FoundStation = Stations.Find(Station->StationID)) {
    if (*FoundStation == Station) {
      Stations.Remove(Station->StationID);
    }
  }
}

URailStationBlockLogic *URailwayManager::FindStationByID(int32 ID) const {
  if (ID == 0) {
    return nullptr;
  }
  if (URailStationBlockLogic *const *FoundStation = Stations.Find(ID)) {
    return *FoundStation;
  }
  return nullptr;
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

bool URailwayManager::LaunchTrain(URailStationBlockLogic *From, int32 TargetStationId) {
  if (!From || TargetStationId == 0) {
    return false;
  }
  UTrainSchedule *Schedule = NewObject<UTrainSchedule>(this);
  if (!Schedule) {
    return false;
  }
  Schedule->AddStopAlways(From->StationID);
  Schedule->AddStopAlways(TargetStationId);
  if (!Schedule->HasEnoughStops()) {
    return false;
  }
  return SpawnTrainWithSchedule(Schedule, 0);
}

bool URailwayManager::SpawnTrainWithSchedule(UTrainSchedule *Schedule, int32 InitialStopIndex) {
  if (!Schedule || !Schedule->HasEnoughStops()) {
    return false;
  }
  if (!Schedule->Stops.IsValidIndex(InitialStopIndex)) {
    return false;
  }
  const int32 InitialStationId = Schedule->Stops[InitialStopIndex].StationId;
  if (!FindStationByID(InitialStationId)) {
    LOG(ERROR_LL) << "RailSpawn: initial stop station not found id=" << InitialStationId;
    return false;
  }

  UTrainInstance *Train = NewObject<UTrainInstance>(this);
  if (!Train) {
    return false;
  }
  Train->Schedule = Schedule;
  Train->CurrentStopIndex = InitialStopIndex;
  Train->CurrentStationId = InitialStationId;
  Train->SimState = ETrainSimState::Arrived;
  Train->Speed = 1200.f;
  Train->InstanceRandom = FMath::Rand();
  Train->Cargo = NewObject<UInventory>(this);
  Train->DepartureContext = NewObject<ULogicContext>(this);
  if (Train->Cargo) {
    Train->Cargo->Resize(1);
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

bool URailwayManager::BuildPathBetweenStations(int32 SourceStationId, int32 TargetStationId, TArray<FRailPathStep> &OutPath) const {
  OutPath.Empty();
  if (!railNetwork.IsValid() || SourceStationId == 0 || TargetStationId == 0 || SourceStationId == TargetStationId) {
    return false;
  }
  URailStationBlockLogic *From = FindStationByID(SourceStationId);
  URailStationBlockLogic *To = FindStationByID(TargetStationId);
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
  URailStationBlockLogic *Station = FindStationByID(Train->CurrentStationId);
  if (!Station) {
    return false;
  }
  UCondition *Condition = Train->Schedule->Stops[Train->CurrentStopIndex].DepartureCondition;
  if (!Condition) {
    return true;
  }
  ULogicContext *Context = Train->DepartureContext;
  if (!Context || !Context->Input) {
    return false;
  }
  Context->Input->Clear();
  if (Station->InputInventory) {
    Context->Input->FromInventory(Station->InputInventory);
  }
  if (Station->OutputInventory) {
    Context->Input->FromInventory(Station->OutputInventory);
  }
  if (Train->Cargo) {
    Context->Input->FromInventory(Train->Cargo);
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
  if (!IsDepartureConditionMet(T)) {
    return false;
  }

  const int32 NextStopIndex = T->Schedule->ResolveNextStopIndex(T->CurrentStopIndex);
  if (!T->Schedule->Stops.IsValidIndex(NextStopIndex)) {
    return false;
  }
  const int32 SourceStationId = T->Schedule->Stops[T->CurrentStopIndex].StationId;
  const int32 TargetStationId = T->Schedule->Stops[NextStopIndex].StationId;
  URailStationBlockLogic *From = FindStationByID(SourceStationId);
  URailStationBlockLogic *To = FindStationByID(TargetStationId);
  if (!From || !To) {
    return false;
  }

  TArray<FRailPathStep> path;
  if (!BuildPathBetweenStations(SourceStationId, TargetStationId, path)) {
    return false;
  }
  T->Path = MoveTemp(path);
  T->PathIndex = 0;
  T->DistanceAlongSegment = 0.f;
  T->TargetStationId = TargetStationId;
  T->SourceStation = From;
  T->TargetStation = To;
  T->SimState = ETrainSimState::Moving;
  if (T->Cargo && From->InputInventory) {
    std::ignore = UInventoryLibrary::TryMoveFromAny(T->Cargo, From->InputInventory);
  }
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
  if (ArrivedStation) {
    if (T->Cargo && ArrivedStation->OutputInventory) {
      std::ignore = UInventoryLibrary::TryMoveFromAny(ArrivedStation->OutputInventory, T->Cargo);
    }
  }
  if (T->Schedule && T->Schedule->Stops.IsValidIndex(ArrivedStopIndex)) {
    T->CurrentStopIndex = ArrivedStopIndex;
    T->CurrentStationId = T->Schedule->Stops[ArrivedStopIndex].StationId;
    T->SimState = ETrainSimState::Arrived;
  } else {
    T->SimState = ETrainSimState::Idle;
  }
  T->Path.Empty();
  T->PathIndex = 0;
  T->DistanceAlongSegment = 0.f;
  T->SourceStation = ArrivedStation;
  T->TargetStation = nullptr;
  T->TargetStationId = 0;
}

void URailwayManager::UpdateTrainMovement(int32 Index, float Dt) {
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
  float len = railNetwork->GetEdgeLength(Step.From, Step.To);
  if (len < KINDA_SMALL_NUMBER) {
    T->PathIndex++;
    T->DistanceAlongSegment = 0.f;
    if (T->PathIndex >= T->Path.Num()) {
      ArriveAtTarget(Index, ArrivedStopIndex);
    }
    return;
  }
  T->DistanceAlongSegment += T->Speed * Dt;
  while (T->DistanceAlongSegment >= len - KINDA_SMALL_NUMBER) {
    T->DistanceAlongSegment -= len;
    T->PathIndex++;
    if (T->PathIndex >= T->Path.Num()) {
      T->DistanceAlongSegment = 0.f;
      ArriveAtTarget(Index, ArrivedStopIndex);
      return;
    }
    const FRailPathStep &Next = T->Path[T->PathIndex];
    len = railNetwork->GetEdgeLength(Next.From, Next.To);
    if (len < KINDA_SMALL_NUMBER) {
      if (Next.From == Next.To) {
        T->SimState = ETrainSimState::NoPath;
        return;
      }
    }
  }
}

bool URailwayManager::TryGetTrainTransform(const UTrainInstance *T, FVector &OutWorldLocation, FRotator &OutWorldRotation) const {
  if (!T) {
    return false;
  }
  static constexpr float kVisualZOffsetUu = 80.f;
  if (railNetwork.IsValid() && T->Path.Num() > 0 && T->PathIndex < T->Path.Num()) {
    const FRailPathStep &S = T->Path[T->PathIndex];
    FVector p, tan;
    railNetwork->SampleEdgeWorld(S.From, S.To, T->DistanceAlongSegment, p, tan);
    OutWorldLocation = p + FVector(0.f, 0.f, kVisualZOffsetUu);
    OutWorldRotation = tan.Rotation();
    return true;
  }

  URailStationBlockLogic *Station = FindStationByID(T->CurrentStationId);
  if (!Station) {
    return false;
  }

  UBlockLogic *StationRoot = Station->GetPartRootBlock();
  if (!StationRoot) {
    StationRoot = Station;
  }
  OutWorldLocation = StationRoot->GetLocation() + FVector(0.f, 0.f, kVisualZOffsetUu);
  OutWorldRotation = StationRoot->GetBlockQuat().Rotator();
  return true;
}

void URailwayManager::Tick(float SimStepSeconds) {
  if (!ownerDimension.IsValid() || !railNetwork.IsValid()) {
    return;
  }
  for (int i = 0; i < Trains.Num(); ++i) {
    if (Trains[i] && Trains[i]->SimState == ETrainSimState::Moving) {
      UpdateTrainMovement(i, SimStepSeconds);
    } else {
      std::ignore = TryDispatchTrainFromSchedule(i);
    }
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
  for (int i = 0; i < Trains.Num(); ++i) {
    UTrainInstance *T = Trains[i];
    if (!T) {
      continue;
    }
    EnsureVisual(i);
    if (ATrainActor *Vis = TrainVisuals[i]) {
      Vis->BindToTrain(this, i);
      FVector at;
      FRotator r;
      if (TryGetTrainTransform(T, at, r)) {
        Vis->SetActorLocationAndRotation(at, r);
      }
    }
  }
  DebugDrawRailGraph();
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

int32 URailwayManager::GenerateStationID() {
  if (NextStationId <= 0) {
    NextStationId = 1;
  }
  while (Stations.Contains(NextStationId)) {
    ++NextStationId;
  }
  const int32 id = NextStationId;
  ++NextStationId;
  return id;
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

static bool ParseStationIdArg(const TArray<FString> &Args, int32 ArgIndex, int32 &OutStationId) {
  if (!Args.IsValidIndex(ArgIndex)) {
    return false;
  }
  OutStationId = FCString::Atoi(*Args[ArgIndex]);
  return OutStationId > 0;
}

static bool BuildScheduleStopIds(const TArray<FString> &Args, const TMap<int32, URailStationBlockLogic *> &Stations, TArray<int32> &OutStopIds) {
  OutStopIds.Empty();
  if (Args.Num() == 0) {
    for (const TPair<int32, URailStationBlockLogic *> &Pair : Stations) {
      if (!Pair.Value) {
        continue;
      }
      OutStopIds.Add(Pair.Key);
      if (OutStopIds.Num() == 2) {
        return true;
      }
    }
    return false;
  }

  for (int32 i = 0; i < Args.Num(); ++i) {
    int32 StationId = 0;
    if (!ParseStationIdArg(Args, i, StationId)) {
      return false;
    }
    OutStopIds.Add(StationId);
  }
  return OutStopIds.Num() >= 2;
}

static FAutoConsoleCommandWithWorldAndArgs RailSpawnTrainCmd(
  TEXT("Evospace.Rail.SpawnTrain"),
  TEXT("Spawn one train with schedule. Usage: Evospace.Rail.SpawnTrain [stationId1 stationId2 ...]"),
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

    const TMap<int32, URailStationBlockLogic *> &Stations = RailwayManager->GetStations();
    if (Stations.Num() < 2) {
      LOG(ERROR_LL) << "RailSpawnTrain: requires at least two registered stations, current=" << Stations.Num();
      return;
    }

    TArray<int32> StopIds;
    if (!BuildScheduleStopIds(Args, Stations, StopIds)) {
      LOG(ERROR_LL) << "RailSpawnTrain: usage Evospace.Rail.SpawnTrain [stationId1 stationId2 ...]";
      return;
    }

    UTrainSchedule *Schedule = NewObject<UTrainSchedule>(RailwayManager);
    if (!Schedule) {
      LOG(ERROR_LL) << "RailSpawnTrain: failed to create schedule";
      return;
    }
    for (const int32 StationId : StopIds) {
      if (!RailwayManager->FindStationByID(StationId)) {
        LOG(ERROR_LL) << "RailSpawnTrain: station not found id=" << StationId;
        return;
      }
      Schedule->AddStopAlways(StationId);
    }
    if (!Schedule->HasEnoughStops()) {
      LOG(ERROR_LL) << "RailSpawnTrain: schedule requires at least two valid stops";
      return;
    }
    if (!RailwayManager->SpawnTrainWithSchedule(Schedule, 0)) {
      LOG(ERROR_LL) << "RailSpawnTrain: failed to spawn train with schedule";
      return;
    }

    LOG(INFO_LL) << "RailSpawnTrain: spawned scheduled train, stops=" << Schedule->Stops.Num();
  }));

} // namespace
