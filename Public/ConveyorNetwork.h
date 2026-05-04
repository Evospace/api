// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "NetworkBase.h"
#include "ConveyorNetwork.generated.h"

class UConveyorBlockLogic;

UCLASS()
class UConveyorNetwork : public UNetworkBase {
  GENERATED_BODY()
  public:
  UPROPERTY(VisibleAnywhere)
  TArray<UConveyorBlockLogic *> Conveyors;
};
