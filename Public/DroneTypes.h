#pragma once

#include "CoreMinimal.h"
#include "Qr/CoordinameMinimal.h"
#include "DroneTypes.generated.h"

class UDroneStationBlockLogic;
class UInventory;

UENUM()
enum class EDroneState : uint8 {
  Idle,
  TravelingToTarget,
  Unloading,
  Returning,
  Count
};

USTRUCT()
struct FDroneInstanceData {
  GENERATED_BODY()

  // --- Deterministic simulation state: integer only, no float (lockstep-safe) ---
  Vec3i LegStart; // block pos where the current leg began
  Vec3i TargetPosition; // block pos of the current leg's destination
  int64 LegLengthUu = 0; // straight-line length of the current leg, in world uu
  int64 TraveledUu = 0; // distance covered along the current leg, in world uu

  TWeakObjectPtr<UDroneStationBlockLogic> Source;
  TWeakObjectPtr<UDroneStationBlockLogic> Target;

  EDroneState State = EDroneState::Idle;

  // --- Presentation only: float is fine here, never feeds sim state ---
  FVector VisualPosition = FVector::ZeroVector; // render position, lags slightly behind the sim position
  int32 InstanceRandom = 0; // per-drone seed for hover/sway

  UPROPERTY()
  UInventory *Payload = nullptr;
};
