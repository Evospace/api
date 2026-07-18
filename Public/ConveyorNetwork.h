// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "NetworkBase.h"
#include "Public/BlockRef.h"
#include "ConveyorNetwork.generated.h"

class ADimension;
class UConveyorBlockLogic;
class UBaseInventoryAccessor;
class UInventoryAccess;

UCLASS()
class UConveyorNetwork : public UNetworkBase {
  GENERATED_BODY()
  public:
  /**
   * When false, conveyor networks do not run simulation (Tick / EndTick).
   * Empty-network removal in ADimension still runs. Cvar: evospace.ConveyorNetwork.Simulate.
   */
  static bool IsSimulationEnabled();

  UPROPERTY(VisibleAnywhere)
  TArray<UConveyorBlockLogic *> Conveyors;

  virtual void Tick() override;

  // Rebuild static topology/cache after network is (re)constructed
  void RebuildCache();

  // Cached topology (stable between rebuilds)
  UPROPERTY(VisibleAnywhere)
  TArray<int32> NeighborIndex; // per conveyor index, -1 if receiver is not a conveyor

  TArray<TBlockRef<UBaseInventoryAccessor>> ReceiverAccessors;

  UPROPERTY(VisibleAnywhere)
  TArray<int32> ReceiverId; // per conveyor, -1 if no receiver

  UPROPERTY(VisibleAnywhere)
  TArray<int32> PostOrder; // downstream-to-upstream processing order

  UPROPERTY(VisibleAnywhere)
  TArray<UInventoryAccess *> CachedInput;

  UPROPERTY(VisibleAnywhere)
  TArray<UInventoryAccess *> CachedOutput;

  struct FConveyorTickState {
    bool InputEmpty = true;
    bool OutputEmpty = true;
    bool InputReady = false;
    bool OutputReady = false;
  };

  TArray<FConveyorTickState> TickStates;
  TArray<int32> ActivePostOrder;

  // Working buffers reused between ticks (to avoid reallocations)
  TArray<UBaseInventoryAccessor *> ResolvedReceivers;
  TArray<uint8> TmpVisited;
  TArray<bool> Accept;
  TArray<bool> OutClears;
  TArray<int32> ChosenSenderByReceiver; // per ReceiverId: Conveyors index (first eligible in ActivePostOrder), or INDEX_NONE
  int32 UniqueReceiverCount = 0;
};
