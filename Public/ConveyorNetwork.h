// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "NetworkBase.h"
#include "ConveyorNetwork.generated.h"

class ADimension;
class UConveyorBlockLogic;
class UBaseInventoryAccessor;
class UInventoryAccess;

UCLASS()
class UConveyorNetwork : public UNetworkBase {
  GENERATED_BODY()
  public:
  UPROPERTY(VisibleAnywhere)
  TArray<UConveyorBlockLogic *> Conveyors;

  virtual void Tick() override;

  // Rebuild static topology/cache after network is (re)constructed
  void RebuildCache();

  // Cached topology (stable between rebuilds)
  UPROPERTY(VisibleAnywhere)
  TArray<int32> NeighborIndex; // per conveyor index, -1 if receiver is not a conveyor

  UPROPERTY(VisibleAnywhere)
  TArray<UBaseInventoryAccessor *> ReceiverAccessors; // per conveyor

  UPROPERTY(VisibleAnywhere)
  TArray<int32> ReceiverId; // per conveyor, -1 if no receiver

  UPROPERTY(VisibleAnywhere)
  TArray<int32> PostOrder; // downstream-to-upstream processing order

  UPROPERTY(VisibleAnywhere)
  TArray<UInventoryAccess *> CachedInput; // per conveyor

  UPROPERTY(VisibleAnywhere)
  TArray<UInventoryAccess *> CachedOutput; // per conveyor

  // Working buffers reused between ticks (to avoid reallocations)
  TArray<uint8> TmpVisited;
  TArray<bool> InputEmpty;
  TArray<bool> OutputEmpty;
  TArray<bool> InputReady;
  TArray<bool> OutputReady;
  TArray<bool> Accept;
  TArray<bool> OutClears;
  TArray<UConveyorBlockLogic *> ChosenSenderByReceiver; // indexed by compact ReceiverId
  int32 UniqueReceiverCount = 0;
};


