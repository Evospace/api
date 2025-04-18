#pragma once
#include "DroneStationBlockLogic.h"
#include "Inventory.h"
#include "Evospace/CoordinameMinimal.h"
#include "DroneManagerActor.generated.h"

class UInventory;
class UDroneStationBlockLogic;

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

UCLASS()
class EVOSPACE_API ADroneManagerActor : public AActor {
  GENERATED_BODY()

  public:
  ADroneManagerActor();

  virtual void Tick(float DeltaTime) override;

  int32 LaunchDrone(UDroneStationBlockLogic *From, UDroneStationBlockLogic *To, UInventory *Payload);
  void RegisterStation(UDroneStationBlockLogic *s);

  UDroneStationBlockLogic *FindStationByID(const FString &string);

  UPROPERTY()
  TArray<FDroneInstanceData> Drones;

  FString GenerateStationID() const;
  bool NameUsed(const FString &BaseName) const;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  TMap<FString, UDroneStationBlockLogic *> Stations;

  protected:
  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  UInstancedStaticMeshComponent *DroneMesh;

  void UpdateDrones(float DeltaTime);
};