#pragma once

#include "CoreMinimal.h"
#include "BlockLogic.h"
#include "DroneStationBlockLogic.generated.h"

class UInventory;
class UDroneManager;
class APlayerController;

/**
 * Anchor block for the (future) construction drone: a player-controlled build
 * pawn deployed from this station and drawing materials from its inventory.
 *
 * The old cargo-drone logistics (routes, auto-dispatch, flying payload drones)
 * were removed — conveyors and trains cover logistics. What remains is the
 * station identity: registration + non-unique naming (shared design with rail
 * stations) and a materials inventory. Deploying the construction pawn will go
 * through the player-action pipeline (not the deterministic sim), so nothing
 * here ticks in the simulation.
 */
UCLASS()
class UDroneStationBlockLogic : public UBlockLogic {
  GENERATED_BODY()

  public:
  UDroneStationBlockLogic();
  virtual UCoreAccessor *CoreInit() override;

  // Materials the construction drone will build from.
  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  UInventory *InputInventory;

  // Non-unique display name; empty = unassigned, a default ("Station N") is assigned
  // in UDroneManager::RegisterStation. Change via RenameStation, not directly.
  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  FString StationName;

  /**
   * Player-facing rename; delegates to UDroneManager::RenameStation (multiplayer
   * sync). Pc non-null marks a local player action.
   */
  UFUNCTION(BlueprintCallable, Category = "Drone|Station")
  bool RenameStation(APlayerController *Pc, const FString &NewName);

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  TWeakObjectPtr<UDroneManager> DroneManager;

  virtual void BlockBeginPlay() override;
  virtual void BlockEndPlay() override;

  virtual EBlockWidgetType GetWidgetType() const override;
  virtual TSubclassOf<UBlockWidget> GetWidgetClass() const override;

  virtual void SaveSettings(TSharedPtr<FJsonObject> json, AMainPlayerController *mpc) const override;
  virtual void LoadSettings(TSharedPtr<FJsonObject> json, AMainPlayerController *mpc) override;
};
