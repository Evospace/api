#pragma once

#include "CoreMinimal.h"
#include "DroneTypes.h"
#include "DroneManager.generated.h"

class ADimension;
class UDroneStationBlockLogic;
class UInstancedStaticMeshComponent;
class UInventory;

UCLASS()
class UDroneManager : public UObject {
  GENERATED_BODY()

  public:
  void Initialize(ADimension *inOwner, UInstancedStaticMeshComponent *inDroneMeshComponent);
  void Tick(float DeltaTime);

  int32 LaunchDrone(UDroneStationBlockLogic *From, UDroneStationBlockLogic *To, UInventory *Payload);
  void RegisterStation(UDroneStationBlockLogic *Station);
  void UnregisterStation(UDroneStationBlockLogic *Station);
  UDroneStationBlockLogic *FindStationByID(const FString &ID);

  FString GenerateStationID() const;
  bool NameUsed(const FString &BaseName) const;

  const TMap<FString, UDroneStationBlockLogic *> &GetStations() const { return Stations; }

  UPROPERTY()
  TArray<FDroneInstanceData> Drones;

  private:
  void UpdateDrones(float DeltaTime);

  UPROPERTY()
  TWeakObjectPtr<ADimension> ownerDimension;

  UPROPERTY()
  UInstancedStaticMeshComponent *DroneMeshComponent = nullptr;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  TMap<FString, UDroneStationBlockLogic *> Stations;
};
