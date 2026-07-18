#pragma once

#include "CoreMinimal.h"
#include "BlockLogic.h"
#include "DroneStationBlockLogic.generated.h"

class UInventory;
class UDroneManager;
class APlayerController;

UCLASS()
class UDroneStationBlockLogic : public UBlockLogic {
  GENERATED_BODY()

  public:
  UDroneStationBlockLogic();
  virtual UCoreAccessor *CoreInit() override;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  UInventory *InputInventory;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  FString StationName;

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
