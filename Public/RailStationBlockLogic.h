// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Public/RailNodeBlockLogic.h"
#include "RailStationBlockLogic.generated.h"

class UInventory;
class UCoreAccessor;
class URailwayManager;
class UBlockWidget;

UCLASS()
class URailStationBlockLogic : public URailNodeBlockLogic {
  GENERATED_BODY()

  public:
  URailStationBlockLogic();
  virtual UCoreAccessor *CoreInit() override;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  UInventory *InputInventory = nullptr;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  UInventory *OutputInventory = nullptr;

  // 0 = unassigned; assigned in URailwayManager::RegisterStation.
  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  int32 StationID = 0;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  TWeakObjectPtr<URailwayManager> RailwayManager;

  virtual void BlockBeginPlay() override;
  virtual void BlockEndPlay() override;

  virtual bool IsBlockTicks() const override { return false; }
  virtual bool IsStation() const override { return true; }

  virtual TSubclassOf<UBlockWidget> GetWidgetClass() const override;
};
