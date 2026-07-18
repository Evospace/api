#pragma once

#include "CoreMinimal.h"
#include "Qr/CoordinameMinimal.h"
#include "DroneManager.generated.h"

class UDroneStationBlockLogic;
class APlayerController;

UCLASS()
class UDroneManager : public UObject {
  GENERATED_BODY()

  public:
  void RegisterStation(UDroneStationBlockLogic *Station);
  void UnregisterStation(UDroneStationBlockLogic *Station);

  UDroneStationBlockLogic *FindStationAt(const Vec3i &Pos) const;

  void FindStationsByName(const FString &Name, TArray<UDroneStationBlockLogic *> &Out) const;

  bool RenameStation(APlayerController *Pc, UDroneStationBlockLogic *Station, const FString &NewName);

  FString GenerateDefaultStationName() const;

  UFUNCTION(BlueprintCallable, Category = "Drone|Station")
  TArray<FString> GetAllStationNames() const;

  const TMap<FQrVector3i, UDroneStationBlockLogic *> &GetStations() const { return Stations; }

  private:
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  TMap<FQrVector3i, UDroneStationBlockLogic *> Stations;
};
