#pragma once

#include "CoreMinimal.h"
#include "DroneTypes.h"
#include "DroneManager.generated.h"

class ADimension;
class UDroneStationBlockLogic;
class UInstancedStaticMeshComponent;
class UInventory;
class APlayerController;

UCLASS()
class UDroneManager : public UObject {
  GENERATED_BODY()

  public:
  void Initialize(ADimension *inOwner, UInstancedStaticMeshComponent *inDroneMeshComponent);

  /** Deterministic logic: movement and arrival, uses world tick delta. Called from TickBlocks. */
  void Tick(float TickDelta);

  /** Visual only: instance transforms with sway/hover. Called from Dimension::Tick. */
  void TickVisual(float DeltaTime);

  int32 LaunchDrone(UDroneStationBlockLogic *From, UDroneStationBlockLogic *To, UInventory *Payload);

  /** Registers by block position; assigns a default name ("Station N") when unnamed. */
  void RegisterStation(UDroneStationBlockLogic *Station);
  void UnregisterStation(UDroneStationBlockLogic *Station);

  UDroneStationBlockLogic *FindStationAt(const Vec3i &Pos) const;

  /** All stations carrying Name, sorted by block position (deterministic across peers). */
  void FindStationsByName(const FString &Name, TArray<UDroneStationBlockLogic *> &Out) const;

  /** Nearest station named Name (integer distance², tie-break by position); Exclude may be null. */
  UDroneStationBlockLogic *FindNearestStationByName(const Vec3i &FromPos, const FString &Name, const UDroneStationBlockLogic *Exclude) const;

  /**
   * Renames a station. Renaming the last station carrying the old name also updates
   * every drone route that referenced it. Pc non-null marks a local player action
   * (replicated to the session); internal/remote callers pass nullptr.
   */
  bool RenameStation(APlayerController *Pc, UDroneStationBlockLogic *Station, const FString &NewName);

  /** Smallest free "Station N"; deterministic function of the registered set. */
  FString GenerateDefaultStationName() const;

  /** Distinct station names, sorted; for route-editing UI. */
  UFUNCTION(BlueprintCallable, Category = "Drone|Station")
  TArray<FString> GetAllStationNames() const;

  const TMap<FQrVector3i, UDroneStationBlockLogic *> &GetStations() const { return Stations; }

  UPROPERTY()
  TArray<FDroneInstanceData> Drones;

  private:
  /** Deterministic: fixed integer step per tick, no delta time. */
  void TickLogic();
  void UpdateVisual(float DeltaTime);

  UPROPERTY()
  TWeakObjectPtr<ADimension> ownerDimension;

  UPROPERTY()
  UInstancedStaticMeshComponent *DroneMeshComponent = nullptr;

  // Keyed by the station block position — the stable identity; names are non-unique labels.
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  TMap<FQrVector3i, UDroneStationBlockLogic *> Stations;
};
