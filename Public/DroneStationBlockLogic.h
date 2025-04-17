#pragma once

#include "CoreMinimal.h"
#include "BlockLogic.h"
#include "DroneRoute.h"
#include "DroneStationBlockLogic.generated.h"

class UInventory;
class ADroneManagerActor;
UCLASS()
class EVOSPACE_API UDroneStationBlockLogic : public UBlockLogic {
  GENERATED_BODY()

  public:
  UDroneStationBlockLogic();

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  UInventory *InputInventory;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  UInventory *OutputInventory;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  TArray<FDroneRoute> Routes;

  // Уникальный ID станции
  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  FString StationID;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  TWeakObjectPtr<ADroneManagerActor> DroneManager;

  virtual void BlockBeginPlay() override;

  virtual bool IsBlockTicks() const override { return true; }
  virtual void Tick() override;

  UFUNCTION(BlueprintCallable)
  void MakeRouteTest();

  void DispatchDroneIfReady();
  void ReceiveDronePayload(UInventory *Payload);

  virtual EBlockWidgetType GetWidgetType() const override;

  virtual TSubclassOf<UBlockWidget> GetWidgetClass() const override;

  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  int Delay = 100;
};