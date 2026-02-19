#include "Public/DroneManager.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Evospace/CoordinateSystem.h"
#include "Public/Dimension.h"
#include "Public/DroneStationBlockLogic.h"
#include "Public/Inventory.h"

void UDroneManager::Initialize(ADimension *inOwner, UInstancedStaticMeshComponent *inDroneMeshComponent) {
  ownerDimension = inOwner;
  DroneMeshComponent = inDroneMeshComponent;
}

void UDroneManager::Tick(float DeltaTime) {
  UpdateDrones(DeltaTime);
}

void UDroneManager::UpdateDrones(float DeltaTime) {
  UWorld *World = ownerDimension.IsValid() ? ownerDimension->GetWorld() : nullptr;
  if (!World || !DroneMeshComponent) {
    return;
  }

  for (int32 i = 0; i < Drones.Num(); ++i) {
    FDroneInstanceData &Drone = Drones[i];

    if (Drone.State == EDroneState::Idle) {
      continue;
    }

    if (!Drone.Source.IsValid() || !Drone.Target.IsValid() || !Drone.Payload) {
      Drones.RemoveAt(i);
      DroneMeshComponent->RemoveInstance(i);
      --i;
      continue;
    }

    FVector Direction = (Drone.TargetPosition.world() - Drone.Position).GetSafeNormal();
    Drone.Position += Direction * Drone.Speed * DeltaTime;

    float HoverOffset = FMath::Sin(World->TimeSeconds * 3.0f + Drone.InstanceRandom) * 10.0f;
    float Sway = FMath::Sin(World->TimeSeconds * 1.2f + Drone.InstanceRandom * 3.14f) * 8.0f;
    FVector VisualPosition = Drone.Position + FVector(0, Sway, 200 + HoverOffset);

    FRotator VisualRotation = Direction.Rotation();
    float RollAmount = FMath::Sin(World->TimeSeconds * 1.5f + Drone.InstanceRandom * 2.17f) * 7.f;
    FRotator RollTilt(0, 0, RollAmount);
    VisualRotation += RollTilt;

    FTransform InstanceTransform = FTransform(VisualRotation, VisualPosition);
    DroneMeshComponent->UpdateInstanceTransform(i, InstanceTransform, true, true);

    if (FVector::DistSquared(Drone.Position, Drone.TargetPosition.world()) < 100.f * 100.f) {
      if (Drone.State == EDroneState::TravelingToTarget) {
        Drone.Target->ReceiveDronePayload(Drone.Payload);
        Drone.TargetPosition = Drone.Source->GetBlockPos();
        Drone.State = EDroneState::Returning;
      } else if (Drone.State == EDroneState::Returning) {
        Drones.RemoveAt(i);
        DroneMeshComponent->RemoveInstance(i);
        --i;
      }
    }
  }
}

int32 UDroneManager::LaunchDrone(UDroneStationBlockLogic *From, UDroneStationBlockLogic *To, UInventory *Payload) {
  static int32 indexCounter = 1;
  if (!From || !To || !Payload || !DroneMeshComponent) {
    return INDEX_NONE;
  }

  FDroneInstanceData Drone;
  Drone.Source = From;
  Drone.Target = To;
  Drone.Payload = Payload;
  Drone.State = EDroneState::TravelingToTarget;
  Drone.InstanceRandom = indexCounter++;
  Drone.Position = From->GetBlockPos().world();
  Drone.TargetPosition = To->GetBlockPos();
  Drone.Speed = 600.f;

  int32 Index = Drones.Num();
  Drones.Add(Drone);

  FTransform Transform;
  Transform.SetLocation(Drone.Position);
  DroneMeshComponent->AddInstance(Transform);

  return Index;
}

void UDroneManager::RegisterStation(UDroneStationBlockLogic *Station) {
  if (!Station || Station->StationID.IsEmpty()) {
    return;
  }
  Stations.Add(Station->StationID, Station);
}

void UDroneManager::UnregisterStation(UDroneStationBlockLogic *Station) {
  if (!Station) {
    return;
  }

  if (!Station->StationID.IsEmpty()) {
    UDroneStationBlockLogic **FoundStation = Stations.Find(Station->StationID);
    if (FoundStation && *FoundStation == Station) {
      Stations.Remove(Station->StationID);
    }
  }
}

UDroneStationBlockLogic *UDroneManager::FindStationByID(const FString &ID) {
  UDroneStationBlockLogic **FoundStation = Stations.Find(ID);
  return FoundStation ? *FoundStation : nullptr;
}

FString UDroneManager::GenerateStationID() const {
  static const TArray<FString> FirstParts = { TEXT("Iron"), TEXT("Copper"), TEXT("Silver"), TEXT("Quantum"), TEXT("Thorn"), TEXT("Crimson"), TEXT("Stone"), TEXT("Titan"), TEXT("Void"), TEXT("Amber") };
  static const TArray<FString> SecondParts = { TEXT("Nest"), TEXT("Field"), TEXT("Hollow"), TEXT("Spire"), TEXT("Forge"), TEXT("Core"), TEXT("Vault"), TEXT("Cradle"), TEXT("Throne"), TEXT("Outpost") };

  FString BaseName;
  int32 Attempts = 0;

  do {
    int32 Index1 = FMath::RandRange(0, FirstParts.Num() - 1);
    int32 Index2 = FMath::RandRange(0, SecondParts.Num() - 1);
    BaseName = FirstParts[Index1] + " " + SecondParts[Index2];
    Attempts++;
  } while (Attempts < 100 && NameUsed(BaseName));

  int32 Counter = 1;
  FString FinalName = BaseName;

  while (Stations.Contains(FinalName)) {
    FinalName = FString::Printf(TEXT("%s_%03d"), *BaseName, Counter);
    Counter++;
  }

  return FinalName;
}

bool UDroneManager::NameUsed(const FString &BaseName) const {
  for (const TPair<FString, UDroneStationBlockLogic *> &Pair : Stations) {
    if (Pair.Key.StartsWith(BaseName)) {
      return true;
    }
  }
  return false;
}
