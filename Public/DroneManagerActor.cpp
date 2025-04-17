#include "DroneManagerActor.h"

#include "DroneStationBlockLogic.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Evospace/CoordinateSystem.h"

ADroneManagerActor::ADroneManagerActor() {
  PrimaryActorTick.bCanEverTick = true;
  DroneMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("DroneMesh"));
  RootComponent = DroneMesh;
}

void ADroneManagerActor::Tick(float DeltaTime) {
  Super::Tick(DeltaTime);
  UpdateDrones(DeltaTime);
}

void ADroneManagerActor::UpdateDrones(float DeltaTime) {
  for (int32 i = 0; i < Drones.Num(); ++i) {
    FDroneInstanceData &Drone = Drones[i];

    if (Drone.State == EDroneState::Idle) continue;
    
    FVector Direction = (Drone.TargetPosition.world() - Drone.Position).GetSafeNormal();
    Drone.Position += Direction * Drone.Speed * DeltaTime;

    // Анимация: колебание + наклон
    float HoverOffset = FMath::Sin(GetWorld()->TimeSeconds * 3.0f + Drone.InstanceRandom) * 10.0f;
    float Sway = FMath::Sin(GetWorld()->TimeSeconds * 1.2f + Drone.InstanceRandom * 3.14f) * 8.0f;
    FVector VisualPosition = Drone.Position + FVector(0, Sway, 200 + HoverOffset);

    // Поворот по направлению
    FRotator VisualRotation = Direction.Rotation();

    // Крен: поворачиваем немного вправо-влево от направления
    float RollAmount = FMath::Sin(GetWorld()->TimeSeconds * 1.5f + Drone.InstanceRandom * 2.17f) * 7.f; // в градусах
    FRotator RollTilt(0, 0, RollAmount);
    VisualRotation += RollTilt;

    // Применяем трансформ
    FTransform InstanceTransform = FTransform(VisualRotation, VisualPosition);
    DroneMesh->UpdateInstanceTransform(i, InstanceTransform, true, true);

    if (FVector::DistSquared(Drone.Position, Drone.TargetPosition.world()) < 100.f * 100.f) {
      if (Drone.State == EDroneState::TravelingToTarget) {
        Drone.Target->ReceiveDronePayload(Drone.Payload);
        Drone.TargetPosition = Drone.Source->GetBlockPos();
        Drone.State = EDroneState::Returning;
      } else if (Drone.State == EDroneState::Returning) {
        Drones.RemoveAt(i);
        DroneMesh->RemoveInstance(i);
        --i;
      }
    }
  }
}

int32 ADroneManagerActor::LaunchDrone(UDroneStationBlockLogic *From, UDroneStationBlockLogic *To, UInventory *Payload) {
  static int indexCounter = 1;
  if (!From || !To || !Payload) return INDEX_NONE;

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
  DroneMesh->AddInstance(Transform);

  return Index;
}

void ADroneManagerActor::RegisterStation(UDroneStationBlockLogic *s) {
  Stations.Add(s->StationID, s);
}

UDroneStationBlockLogic * ADroneManagerActor::FindStationByID(const FString &string){
  auto st = Stations.Find(string);
  return st ? *st : nullptr;
}

FString ADroneManagerActor::GenerateStationID() const {
  static const TArray<FString> FirstParts = {
    TEXT("Iron"), TEXT("Copper"), TEXT("Silver"), TEXT("Quantum"), TEXT("Thorn"), TEXT("Crimson"), TEXT("Stone"), TEXT("Titan"), TEXT("Void"), TEXT("Amber")
  };

  static const TArray<FString> SecondParts = {
    TEXT("Nest"), TEXT("Field"), TEXT("Hollow"), TEXT("Spire"), TEXT("Forge"), TEXT("Core"), TEXT("Vault"), TEXT("Cradle"), TEXT("Throne"), TEXT("Outpost")
  };

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

bool ADroneManagerActor::NameUsed(const FString &BaseName) const {
  for (const auto &Pair : Stations) {
    if (Pair.Key.StartsWith(BaseName))
      return true;
  }
  return false;
}