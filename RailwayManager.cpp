// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#include "Public/RailwayManager.h"
#include "Public/BlockLogic.h"
#include "Public/Dimension.h"
#include "Public/Inventory.h"
#include "Public/RailNetwork.h"
#include "Public/RailNodeBlockLogic.h"
#include "Public/RailStationBlockLogic.h"
#include "Public/TrainActor.h"
#include "Evospace/Item/InventoryLibrary.h"
#include "DrawDebugHelpers.h"
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
  if (Stations.Num() >= 2 && bEnableAutoLaunch && AutoLaunchIntervalSeconds > 0.f) {
    AutoLaunchAccumulator = AutoLaunchIntervalSeconds;
  }
}

void URailwayManager::OnRailGraphChanged() {
  if (bEnableAutoLaunch && AutoLaunchIntervalSeconds > 0.f) {
    AutoLaunchAccumulator = AutoLaunchIntervalSeconds;
  }
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
  if (!From || !railNetwork.IsValid() || TargetStationId == 0) {
    return false;
  }
  URailStationBlockLogic *To = FindStationByID(TargetStationId);
  if (!To) {
    return false;
  }
  const FQrVector3i a = StationRootKey(From);
  const FQrVector3i b = StationRootKey(To);
  TArray<FQrVector3i> keyPath;
  if (!railNetwork->FindPathBlockToBlock(a, b, keyPath)) {
    LOG(ERROR_LL) << "RailLaunch FindPath failed " << a.ToString() << " -> " << b.ToString() << " links=" << railNetwork->GetUndirectedLinkCount() << " targetId=" << TargetStationId;
    return false;
  }
  TArray<FRailPathStep> path;
  railNetwork->BuildPathStepsFromKeyPath(keyPath, path);
  if (path.Num() == 0) {
    return false;
  }
  FTrainInstanceData T;
  T.Path = path;
  T.PathIndex = 0;
  T.DistanceAlongSegment = 0.f;
  T.SimState = ETrainSimState::Moving;
  T.TargetStationId = TargetStationId;
  T.SourceStation = From;
  T.TargetStation = To;
  T.Speed = 1200.f;
  T.InstanceRandom = FMath::Rand();
  T.Cargo = NewObject<UInventory>(this);
  if (T.Cargo) {
    T.Cargo->Resize(1);
  }
  if (From->InputInventory) {
    std::ignore = UInventoryLibrary::TryMoveFromAny(T.Cargo, From->InputInventory);
  }
  Trains.Add(MoveTemp(T));
  TrainVisuals.Add(nullptr);
  // Visual is tied to “route started”, not to the next ADimension::Tick; avoids ordering vs FTSTicker.
  EnsureVisual(Trains.Num() - 1);
  return true;
}

void URailwayManager::ArriveAtTarget(int32 Index) {
  if (!Trains.IsValidIndex(Index)) {
    return;
  }
  FTrainInstanceData &T = Trains[Index];
  if (URailStationBlockLogic *To = T.TargetStation.Get()) {
    if (T.Cargo && To->OutputInventory) {
      std::ignore = UInventoryLibrary::TryMoveFromAny(To->OutputInventory, T.Cargo);
    }
  }
  T.SimState = ETrainSimState::Idle;
  T.Path.Empty();
  T.PathIndex = 0;
  T.Cargo = nullptr;
  ReleaseVisual(Index);
}

void URailwayManager::UpdateTrainMovement(int32 Index, float Dt) {
  if (!Trains.IsValidIndex(Index) || !railNetwork.IsValid()) {
    return;
  }
  FTrainInstanceData &T = Trains[Index];
  if (T.Path.Num() == 0) {
    T.SimState = ETrainSimState::NoPath;
    return;
  }
  if (T.PathIndex >= T.Path.Num()) {
    ArriveAtTarget(Index);
    return;
  }
  const FRailPathStep &Step = T.Path[T.PathIndex];
  if (Step.From == Step.To) {
    T.SimState = ETrainSimState::NoPath;
    return;
  }
  float len = railNetwork->GetEdgeLength(Step.From, Step.To);
  if (len < KINDA_SMALL_NUMBER) {
    T.PathIndex++;
    T.DistanceAlongSegment = 0.f;
    if (T.PathIndex >= T.Path.Num()) {
      ArriveAtTarget(Index);
    }
    return;
  }
  T.DistanceAlongSegment += T.Speed * Dt;
  while (T.DistanceAlongSegment >= len - KINDA_SMALL_NUMBER) {
    T.DistanceAlongSegment -= len;
    T.PathIndex++;
    if (T.PathIndex >= T.Path.Num()) {
      T.DistanceAlongSegment = 0.f;
      ArriveAtTarget(Index);
      return;
    }
    const FRailPathStep &Next = T.Path[T.PathIndex];
    len = railNetwork->GetEdgeLength(Next.From, Next.To);
    if (len < KINDA_SMALL_NUMBER) {
      if (Next.From == Next.To) {
        T.SimState = ETrainSimState::NoPath;
        return;
      }
    }
  }
}

bool URailwayManager::TryGetTrainVisualTransform(const FTrainInstanceData &T, FVector &OutWorldLocation, FRotator &OutWorldRotation) const {
  if (!railNetwork.IsValid() || T.Path.Num() == 0 || T.PathIndex >= T.Path.Num()) {
    return false;
  }
  const FRailPathStep &S = T.Path[T.PathIndex];
  FVector p, tan;
  railNetwork->SampleEdgeWorld(S.From, S.To, T.DistanceAlongSegment, p, tan);
  static constexpr float kVisualZOffsetUu = 80.f;
  OutWorldLocation = p + FVector(0.f, 0.f, kVisualZOffsetUu);
  OutWorldRotation = tan.Rotation();
  return true;
}

void URailwayManager::Tick(float SimStepSeconds) {
  if (!ownerDimension.IsValid() || !railNetwork.IsValid()) {
    return;
  }
  for (int i = 0; i < Trains.Num(); ++i) {
    if (Trains[i].SimState == ETrainSimState::Moving) {
      UpdateTrainMovement(i, SimStepSeconds);
    }
  }
}

void URailwayManager::MaybeAutoLaunchFrame(float DeltaTime) {
  if (!bEnableAutoLaunch || AutoLaunchIntervalSeconds <= 0.f) {
    return;
  }
  if (Stations.Num() < 2) {
    return;
  }
  for (const FTrainInstanceData &E : Trains) {
    if (E.SimState == ETrainSimState::Moving) {
      return;
    }
  }
  // Once per game frame (TickVisual from ADimension::Tick), not per TickBlocks substep.
  // Running LaunchTrain/FindPath from the FTSTicker substep loop is wrong tick context and
  // can fire many pathfinds per frame; keep policy dispatch on the frame clock.
  AutoLaunchAccumulator += DeltaTime;
  if (AutoLaunchAccumulator < AutoLaunchIntervalSeconds) {
    return;
  }
  AutoLaunchAccumulator = 0.f;
  TArray<URailStationBlockLogic *> List;
  for (const TPair<int32, URailStationBlockLogic *> &K : Stations) {
    if (K.Value) {
      List.Add(K.Value);
    }
  }
  if (List.Num() < 2) {
    return;
  }
  std::ignore = LaunchTrain(List[0], List[1]->StationID);
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
  const FTrainInstanceData &TD = Trains[Index];
  FVector at;
  FRotator Rot;
  if (!TryGetTrainVisualTransform(TD, at, Rot)) {
    return;
  }
  FActorSpawnParameters sp;
  sp.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
  sp.Owner = D;
  ATrainActor *a = D->GetWorld()->SpawnActor<ATrainActor>(at, Rot, sp);
  if (a) {
    a->SetActorEnableCollision(false);
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
  if (!ownerDimension.IsValid() || !railNetwork.IsValid()) {
    return;
  }
  MaybeAutoLaunchFrame(DeltaTime);
  for (int i = 0; i < Trains.Num(); ++i) {
    FTrainInstanceData &T = Trains[i];
    if (T.SimState != ETrainSimState::Moving) {
      ReleaseVisual(i);
      continue;
    }
    EnsureVisual(i);
    if (ATrainActor *Vis = TrainVisuals[i]) {
      FVector at;
      FRotator r;
      if (TryGetTrainVisualTransform(T, at, r)) {
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
