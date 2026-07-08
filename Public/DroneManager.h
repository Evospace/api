#pragma once

#include "CoreMinimal.h"
#include "Qr/CoordinameMinimal.h"
#include "DroneManager.generated.h"

class UDroneStationBlockLogic;
class APlayerController;

/**
 * Station registry for drone stations. The cargo-drone logistics mechanic was
 * removed; what remains is the station identity/naming layer that the future
 * construction-drone feature (a player-controlled build pawn tied to a station)
 * will build on. This is not part of the deterministic simulation and does not tick.
 */
UCLASS()
class UDroneManager : public UObject {
  GENERATED_BODY()

  public:
  /** Registers by block position; assigns a default name ("Station N") when unnamed. */
  void RegisterStation(UDroneStationBlockLogic *Station);
  void UnregisterStation(UDroneStationBlockLogic *Station);

  UDroneStationBlockLogic *FindStationAt(const Vec3i &Pos) const;

  /** All stations carrying Name, sorted by block position (deterministic across peers). */
  void FindStationsByName(const FString &Name, TArray<UDroneStationBlockLogic *> &Out) const;

  /**
   * Renames a station. Renaming the last station carrying the old name is a plain
   * rename (no routes to follow anymore). Pc non-null marks a local player action
   * (replicated to the session); internal/remote callers pass nullptr.
   */
  bool RenameStation(APlayerController *Pc, UDroneStationBlockLogic *Station, const FString &NewName);

  /** Smallest free "Station N"; deterministic function of the registered set. */
  FString GenerateDefaultStationName() const;

  /** Distinct station names, sorted; for station-selection UI. */
  UFUNCTION(BlueprintCallable, Category = "Drone|Station")
  TArray<FString> GetAllStationNames() const;

  const TMap<FQrVector3i, UDroneStationBlockLogic *> &GetStations() const { return Stations; }

  private:
  // Keyed by the station block position — the stable identity; names are non-unique labels.
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  TMap<FQrVector3i, UDroneStationBlockLogic *> Stations;
};
