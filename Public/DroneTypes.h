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

  FVector Position;
  Vec3i TargetPosition;

  TWeakObjectPtr<UDroneStationBlockLogic> Source;
  TWeakObjectPtr<UDroneStationBlockLogic> Target;

  EDroneState State = EDroneState::Idle;
  float Speed = 600.f;
  float UnloadTimer = 0.0f;

  int32 InstanceRandom = 0;

  UPROPERTY()
  UInventory *Payload = nullptr;
};
