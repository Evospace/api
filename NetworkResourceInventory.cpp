// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#include "Public/NetworkResourceInventory.h"

#include "Public/ConductorBlockLogic.h"

namespace {

UBlockNetwork *ResolveNetwork(const UConductorBlockLogic *ownerBlock) {
  return ownerBlock ? ownerBlock->GetNetwork() : nullptr;
}

USubnetwork *ResolveSubnetwork(const UConductorBlockLogic *ownerBlock) {
  if (!ownerBlock) {
    return nullptr;
  }

  if (UBlockNetwork *network = ownerBlock->GetNetwork()) {
    const int32 subIndex = ownerBlock->Subnetwork;
    if (subIndex >= 0 && subIndex < network->mSubnetworks.Num()) {
      return network->mSubnetworks[subIndex];
    }
  }

  return nullptr;
}

} // namespace

void UNetworkResourceInventory::Initialize(UConductorBlockLogic *inOwnerBlock) {
  OwnerBlock = inOwnerBlock;

  Data.SetNum(1);

  const USubnetwork *sub = ResolveSubnetwork(OwnerBlock);
  const UBlockNetwork *network = ResolveNetwork(OwnerBlock);

  const int64 subCapacity = sub ? sub->Capacity : 0;
  const int64 subCharge = sub ? sub->Charge : 0;

  Capacity = subCapacity;

  CachedSlot.mItem = network ? network->GetResource() : nullptr;
  CachedSlot.mValue = subCharge;
}

bool UNetworkResourceInventory::IsEmpty() const {
  const USubnetwork *sub = ResolveSubnetwork(OwnerBlock);
  if (!sub) {
    return true;
  }

  return sub->Charge <= 0;
}

const FItemData &UNetworkResourceInventory::_Get(int32 index) const {
  if (index != 0) {
    static FItemData empty;
    return empty;
  }

  const USubnetwork *sub = ResolveSubnetwork(OwnerBlock);
  const UBlockNetwork *network = ResolveNetwork(OwnerBlock);
  if (!sub) {
    static FItemData empty;
    return empty;
  }

  CachedSlot.mItem = network ? network->GetResource() : nullptr;
  CachedSlot.mValue = sub->Charge;
  return CachedSlot;
}

const FItemData &UNetworkResourceInventory::_SafeGet(int32 index) const {
  return _Get(index);
}

const TArray<FItemData> &UNetworkResourceInventory::GetSlots() const {
  static const TArray<FItemData> emptySlots;

  const USubnetwork *sub = ResolveSubnetwork(OwnerBlock);
  const UBlockNetwork *network = ResolveNetwork(OwnerBlock);

  if (!sub) {
    return emptySlots;
  }

  UNetworkResourceInventory *self = const_cast<UNetworkResourceInventory *>(this);

  if (self->Data.Num() != 1) {
    self->Data.SetNum(1);
  }

  FItemData &slot = self->Data[0];
  slot.mItem = network ? network->GetResource() : nullptr;
  slot.mValue = sub->Charge;

  return self->Data;
}

int64 UNetworkResourceInventory::GetInput() const {
  const USubnetwork *sub = ResolveSubnetwork(OwnerBlock);
  if (!sub) {
    return 0;
  }

  return sub->Capacity;
}

int64 UNetworkResourceInventory::GetOutput() const {
  const USubnetwork *sub = ResolveSubnetwork(OwnerBlock);
  if (!sub) {
    return 0;
  }

  return sub->Capacity;
}

int64 UNetworkResourceInventory::GetFreeInput() const {
  const USubnetwork *sub = ResolveSubnetwork(OwnerBlock);
  if (!sub) {
    return 0;
  }

  const int64 capacity = sub->Capacity;
  const int64 charge = sub->Charge;
  const int64 value = capacity * 2 - charge;
  return value > 0 ? value : 0;
}

int64 UNetworkResourceInventory::GetAvailableOutput() const {
  const USubnetwork *sub = ResolveSubnetwork(OwnerBlock);
  if (!sub) {
    return 0;
  }

  const int64 capacity = sub->Capacity;
  const int64 charge = sub->Charge;
  return FMath::Min(capacity != INDEX_NONE ? capacity : charge, charge);
}
