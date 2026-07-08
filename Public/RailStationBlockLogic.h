// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Public/RailNodeBlockLogic.h"
#include "RailStationBlockLogic.generated.h"

class UInventory;
class UCoreAccessor;
class URailwayManager;
class UBlockWidget;
class UTrainInstance;
class UItemMap;
class ULogicContext;
class ULogicSettings;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRailStationTrainDockEvent, UTrainInstance *, Train);

UCLASS()
class URailStationBlockLogic : public URailNodeBlockLogic {
  GENERATED_BODY()

  public:
  URailStationBlockLogic();
  virtual UCoreAccessor *CoreInit() override;
  virtual ULogicSettings *GetLogicSettings() override;
  virtual void LoadSettings(TSharedPtr<FJsonObject> json, AMainPlayerController *mpc = nullptr) override;
  virtual void SaveSettings(TSharedPtr<FJsonObject> json, AMainPlayerController *mpc = nullptr) const override;

  // When no train: 0-slot stub. When docked: train's Cargo (see SetDockedTrain).
  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  UInventory *WagonStorageProxy = nullptr;

  // Empty = unassigned; assigned in URailwayManager::RegisterStation.
  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  FString StationID;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  TWeakObjectPtr<URailwayManager> RailwayManager;

  // Used when no train is at the platform; WagonStorageProxy points here in that case.
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UInventory *StubInventory = nullptr;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UTrainInstance *DockedTrain = nullptr;

  /** Game ticks accumulated while a train is docked; reset when docking state changes. */
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  int64 StationDwellTicks = 0;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  ULogicContext *Context = nullptr;

  virtual void PopulateLogicOutput(class ULogicContext *ctx) const override;
  virtual void ApplyLogicInput(const class ULogicContext *ctx) override;

  void SetDockedTrain(UTrainInstance *Train);
  UTrainInstance *GetDockedTrain() const { return DockedTrain; }

  /** Cosmetic hooks for station animation (gates, lights); fired from SetDockedTrain transitions. */
  UPROPERTY(BlueprintAssignable, Category = "Rail|Events")
  FRailStationTrainDockEvent OnTrainDocked;

  UPROPERTY(BlueprintAssignable, Category = "Rail|Events")
  FRailStationTrainDockEvent OnTrainUndocked;

  /** Exports + wire Context::Input into the docked train DepartureContext for DepartureCondition. */
  void SyncDepartureContextForDockedTrain();

  virtual void BlockBeginPlay() override;
  virtual void BlockEndPlay() override;

  virtual void Tick() override;

  virtual bool IsBlockTicks() const override { return true; }
  virtual bool IsStation() const override { return true; }

  virtual TSubclassOf<UBlockWidget> GetWidgetClass() const override;

  private:
  void FillEnabledLogicExport(UItemMap *Dest) const;
};
