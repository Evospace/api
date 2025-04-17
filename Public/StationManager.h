#pragma once

#include "StationManager.generated.h"

class UDroneStationBlockLogic;

UCLASS()
class UStationManager : public UObject {
  GENERATED_BODY()
  public:
  static UStationManager *Get();

  FVector GetStationPosition(const FString &ID) const;
  UDroneStationBlockLogic *GetStationByID(const FString &ID) const;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  TMap<FString, UDroneStationBlockLogic *> RegisteredStations;
};