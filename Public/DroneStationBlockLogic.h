#pragma once

#include "CoreMinimal.h"
#include "BlockLogic.h"
#include "ContextProvider.h"
#include "DroneRoute.h"
#include "DroneStationBlockLogic.generated.h"

class UCondition;
class UInventory;
class UDroneManager;
UCLASS()
class UDroneStationBlockLogic : public UBlockLogic, public ILogicContextProvider {
  GENERATED_BODY()

  public:
  UDroneStationBlockLogic();
  virtual UCoreAccessor *CoreInit() override;
  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  UInventory *InputInventory;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  UInventory *OutputInventory;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  TArray<FDroneRoute> Routes;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  UCondition *Condition;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  ULogicContext *Context;

  // Уникальный ID станции
  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  FString StationID;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  TWeakObjectPtr<UDroneManager> DroneManager;

  virtual void BlockBeginPlay() override;
  virtual void BlockDestruction() override;

  virtual bool IsBlockTicks() const override { return true; }
  virtual void Tick() override;

  UFUNCTION(BlueprintCallable)
  void MakeRouteTest();

  void DispatchDroneIfReady();
  void ReceiveDronePayload(UInventory *Payload);

  virtual EBlockWidgetType GetWidgetType() const override;

  virtual TSubclassOf<UBlockWidget> GetWidgetClass() const override;

  virtual void SaveSettings(TSharedPtr<FJsonObject> json, AMainPlayerController *mpc) const override;
  virtual void LoadSettings(TSharedPtr<FJsonObject> json, AMainPlayerController *mpc) override;
  virtual ULogicContext *GetContext_Implementation() const override;

  int Delay = 100;
};