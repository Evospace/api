// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once

#include "ResourceInventory.h"

#include "NetworkResourceInventory.generated.h"

class UConductorBlockLogic;

/**
 * Inventory view for a conductor block that reflects its current network
 * subnetwork Charge/Capacity. It does not own storage; it reads values from
 * the owning block's UBlockNetwork/USubnetwork on demand.
 */
UCLASS()
class UNetworkResourceInventory : public UResourceInventory {
  GENERATED_BODY()

  public:
  // Bind this virtual inventory to a specific conductor block. All values are
  // resolved dynamically via the block's network and Subnetwork index.
  void Initialize(UConductorBlockLogic *inOwnerBlock);

  virtual bool IsEmpty() const override;

  virtual const FItemData &_Get(int32 index) const override;
  virtual const FItemData &_SafeGet(int32 index) const override;

  virtual const TArray<FItemData> &GetSlots() const override;

  // Always report dynamic contents for network-backed storage
  virtual int64 GetVersion() const override { return -1; }

  virtual int64 GetInput() const override;
  virtual int64 GetOutput() const override;
  virtual int64 GetFreeInput() const override;
  virtual int64 GetAvailableOutput() const override;

  // Read-only accessor for diagnostics
  UConductorBlockLogic *GetOwnerBlock() const { return OwnerBlock; }

  protected:
  UPROPERTY(VisibleAnywhere)
  UConductorBlockLogic *OwnerBlock = nullptr;

  // Backing slot for _Get/_SafeGet
  mutable FItemData CachedSlot;
};


