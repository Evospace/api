#include "Public/DroneManager.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Evospace/CoordinateSystem.h"
#include "Public/Dimension.h"
#include "Public/DroneStationBlockLogic.h"
#include "Public/Inventory.h"
#include "Evospace/Online/Actions/RailNetActions.h"
#include "Public/StationNameUtils.h"

namespace {
// Cruise speed in world uu per simulation tick. Deterministic: a fixed integer
// step every tick, independent of frame/tick delta time. 30 uu/tick is ~600 uu/s
// at 20 TPS (the drone's former visual speed).
constexpr int64 kDroneSpeedPerTick = 30;

// Integer floor(sqrt(Value)) — keeps leg-length computation deterministic
// (no float sqrt) so arrival is bit-identical across machines.
int64 IntSqrtFloor(uint64 Value) {
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
  return static_cast<int64>(answer);
}

// Straight-line length between two block positions, in world uu (1 block = gCubeSize uu).
int64 LegLengthUuBetween(const Vec3i &A, const Vec3i &B) {
  const int64 dx = static_cast<int64>(B.X) - A.X;
  const int64 dy = static_cast<int64>(B.Y) - A.Y;
  const int64 dz = static_cast<int64>(B.Z) - A.Z;
  const int64 uuPerBlock = static_cast<int64>(gCubeSize);
  const int64 sqUu = (dx * dx + dy * dy + dz * dz) * uuPerBlock * uuPerBlock;
  return IntSqrtFloor(static_cast<uint64>(sqUu));
}

// Begin a new leg from Start to End; resets progress (sim state, pure integer)
// and re-syncs the render cache to the leg start so every leg begins the same
// way (no carry-over gap that would make the render dart to catch up).
// Start/End are taken by value: callers pass Drone.TargetPosition, which this
// function overwrites — a reference param would alias and corrupt the length.
void StartLeg(FDroneInstanceData &Drone, Vec3i Start, Vec3i End) {
  Drone.LegStart = Start;
  Drone.TargetPosition = End;
  Drone.LegLengthUu = LegLengthUuBetween(Start, End);
  Drone.TraveledUu = 0;
  Drone.VisualPosition = Start.world();
}
} // namespace

void UDroneManager::Initialize(ADimension *inOwner, UInstancedStaticMeshComponent *inDroneMeshComponent) {
  ownerDimension = inOwner;
  DroneMeshComponent = inDroneMeshComponent;
}

void UDroneManager::Tick(float TickDelta) {
  TickLogic();
}

void UDroneManager::TickVisual(float DeltaTime) {
  UpdateVisual(DeltaTime);
}

void UDroneManager::TickLogic() {
  if (!DroneMeshComponent) {
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

    // Fixed integer step per tick; progress accumulates until it reaches the leg
    // length. The integer >= test always fires (no float overshoot/oscillation),
    // so a returning drone is always removed and its instance released.
    Drone.TraveledUu += kDroneSpeedPerTick;
    if (Drone.TraveledUu < Drone.LegLengthUu) {
      continue; // still en route
    }

    if (Drone.State == EDroneState::TravelingToTarget) {
      Drone.Target->ReceiveDronePayload(Drone.Payload);
      StartLeg(Drone, Drone.TargetPosition, Drone.Source->GetBlockPos());
      Drone.State = EDroneState::Returning;
    } else if (Drone.State == EDroneState::Returning) {
      Drones.RemoveAt(i);
      DroneMeshComponent->RemoveInstance(i);
      --i;
    }
  }
}

void UDroneManager::UpdateVisual(float DeltaTime) {
  UWorld *World = ownerDimension.IsValid() ? ownerDimension->GetWorld() : nullptr;
  if (!World || !DroneMeshComponent) {
    return;
  }

  for (int32 i = 0; i < Drones.Num(); ++i) {
    FDroneInstanceData &Drone = Drones[i];

    if (Drone.State == EDroneState::Idle) {
      continue;
    }

    // Reconstruct the authoritative sim position from the integer leg progress
    // (presentation math — float is fine here and never feeds sim state).
    const FVector LegStartWorld = Drone.LegStart.world();
    const FVector LegEndWorld = Drone.TargetPosition.world();
    const float Alpha = Drone.LegLengthUu > 0
                          ? FMath::Clamp(static_cast<float>(Drone.TraveledUu) / static_cast<float>(Drone.LegLengthUu), 0.f, 1.f)
                          : 1.f;
    const FVector SimWorld = FMath::Lerp(LegStartWorld, LegEndWorld, Alpha);

    // VisualPosition lags slightly behind the sim position for smooth rendering,
    // then arrives (and disappears) together with the sim drone.
    Drone.VisualPosition = FMath::VInterpTo(Drone.VisualPosition, SimWorld, DeltaTime, 8.0f);

    FVector Direction = (LegEndWorld - LegStartWorld).GetSafeNormal();
    if (Direction.IsNearlyZero()) {
      Direction = FVector::ForwardVector;
    }
    float HoverOffset = FMath::Sin(World->TimeSeconds * 3.0f + Drone.InstanceRandom) * 10.0f;
    float Sway = FMath::Sin(World->TimeSeconds * 1.2f + Drone.InstanceRandom * 3.14f) * 8.0f;
    FVector RenderedPosition = Drone.VisualPosition + FVector(0, Sway, 200 + HoverOffset);

    FRotator VisualRotation = Direction.Rotation();
    float RollAmount = FMath::Sin(World->TimeSeconds * 1.5f + Drone.InstanceRandom * 2.17f) * 7.f;
    FRotator RollTilt(0, 0, RollAmount);
    VisualRotation += RollTilt;

    FTransform InstanceTransform = FTransform(VisualRotation, RenderedPosition);
    DroneMeshComponent->UpdateInstanceTransform(i, InstanceTransform, true, true);
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
  StartLeg(Drone, From->GetBlockPos(), To->GetBlockPos());

  int32 Index = Drones.Num();
  Drones.Add(Drone);

  FTransform Transform;
  Transform.SetLocation(Drone.VisualPosition);
  DroneMeshComponent->AddInstance(Transform);

  return Index;
}

void UDroneManager::RegisterStation(UDroneStationBlockLogic *Station) {
  if (!Station) {
    return;
  }
  if (Station->StationName.IsEmpty()) {
    Station->StationName = GenerateDefaultStationName();
  }
  Stations.Add(Station->GetBlockPos(), Station);
}

void UDroneManager::UnregisterStation(UDroneStationBlockLogic *Station) {
  if (!Station) {
    return;
  }
  if (UDroneStationBlockLogic **FoundStation = Stations.Find(Station->GetBlockPos())) {
    if (*FoundStation == Station) {
      Stations.Remove(Station->GetBlockPos());
    }
  }
}

UDroneStationBlockLogic *UDroneManager::FindStationAt(const Vec3i &Pos) const {
  UDroneStationBlockLogic *const *FoundStation = Stations.Find(Pos);
  return FoundStation ? *FoundStation : nullptr;
}

void UDroneManager::FindStationsByName(const FString &Name, TArray<UDroneStationBlockLogic *> &Out) const {
  Out.Reset();
  if (Name.IsEmpty()) {
    return;
  }
  for (const TPair<FQrVector3i, UDroneStationBlockLogic *> &Pair : Stations) {
    if (Pair.Value && Pair.Value->StationName == Name) {
      Out.Add(Pair.Value);
    }
  }
  // TMap iteration order is insertion-dependent; sort so every peer sees the same order.
  Out.Sort([](const UDroneStationBlockLogic &A, const UDroneStationBlockLogic &B) {
    return A.GetBlockPos() < B.GetBlockPos();
  });
}

UDroneStationBlockLogic *UDroneManager::FindNearestStationByName(const Vec3i &FromPos, const FString &Name, const UDroneStationBlockLogic *Exclude) const {
  TArray<UDroneStationBlockLogic *> Candidates;
  FindStationsByName(Name, Candidates);
  UDroneStationBlockLogic *Best = nullptr;
  int64 BestDistSq = 0;
  for (UDroneStationBlockLogic *Candidate : Candidates) {
    if (Candidate == Exclude) {
      continue;
    }
    const Vec3i Delta = Candidate->GetBlockPos() - FromPos;
    const int64 DistSq = static_cast<int64>(Delta.X) * Delta.X + static_cast<int64>(Delta.Y) * Delta.Y + static_cast<int64>(Delta.Z) * Delta.Z;
    // Candidates arrive position-sorted, so on equal distance the first one wins deterministically.
    if (!Best || DistSq < BestDistSq) {
      Best = Candidate;
      BestDistSq = DistSq;
    }
  }
  return Best;
}

bool UDroneManager::RenameStation(APlayerController *Pc, UDroneStationBlockLogic *Station, const FString &NewName) {
  if (!Station || FindStationAt(Station->GetBlockPos()) != Station) {
    return false;
  }
  const FString Trimmed = NewName.TrimStartAndEnd();
  if (Trimmed.IsEmpty() || Trimmed == Station->StationName) {
    return false;
  }
  const FString OldName = Station->StationName;

  // Renaming the last station carrying the old name follows it through every route
  // (Factorio behavior for a unique station); with sister stations left behind the
  // routes keep pointing at them.
  TArray<UDroneStationBlockLogic *> SameName;
  FindStationsByName(OldName, SameName);
  const bool bLastWithOldName = SameName.Num() == 1 && SameName[0] == Station;

  Station->StationName = Trimmed;

  if (bLastWithOldName) {
    for (const TPair<FQrVector3i, UDroneStationBlockLogic *> &Pair : Stations) {
      if (!Pair.Value) {
        continue;
      }
      for (FDroneRoute &Route : Pair.Value->Routes) {
        if (Route.TargetStationName == OldName) {
          Route.TargetStationName = Trimmed;
        }
      }
    }
  }

  if (Pc) {
    FStationRenameAction Action;
    const auto Pos = Station->GetBlockPos();
    Action.Pos = FIntVector(Pos.X, Pos.Y, Pos.Z);
    Action.NewName = Trimmed;
    SubmitNetAction(Pc, Action);
  }
  return true;
}

FString UDroneManager::GenerateDefaultStationName() const {
  return EvoStationName::SmallestFreeDefaultName(GetAllStationNames());
}

TArray<FString> UDroneManager::GetAllStationNames() const {
  TArray<FString> Names;
  for (const TPair<FQrVector3i, UDroneStationBlockLogic *> &Pair : Stations) {
    if (Pair.Value && !Pair.Value->StationName.IsEmpty()) {
      Names.AddUnique(Pair.Value->StationName);
    }
  }
  Names.Sort();
  return Names;
}
