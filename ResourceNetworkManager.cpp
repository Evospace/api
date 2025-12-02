// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#include "Public/ResourceNetworkManager.h"

#include "Public/Dimension.h"
#include "Public/ConductorBlockLogic.h"

void UResourceNetworkManager::Initialize(ADimension *inOwner) {
  ownerDimension = inOwner;
}

UBlockNetwork *UResourceNetworkManager::CreateNetwork(bool bIsDataNetwork) {
  if (!ownerDimension.IsValid()) {
    return nullptr;
  }

  auto network = NewObject<UBlockNetwork>(ownerDimension.Get());
  if (!network) {
    return nullptr;
  }

  network->mIsDataNetwork = bIsDataNetwork;
  networks.Add(network);

  return network;
}

void UResourceNetworkManager::KillNetworkDeferred(UBlockNetwork *network) {
  if (!network) {
    return;
  }
  if (!network->IsKillDeffered()) {
    networksToKill.Add(network);
    network->DeferredKill();
  }
}

void UResourceNetworkManager::Tick() {
  // Cleanup dead or empty networks
  for (auto network : networks) {
    if (!network) {
      networksToKill.Add(network);
      continue;
    }

    if (network->IsEmpty()) {
      networksToKill.Add(network);
    }
  }

  if (networksToKill.Num() > 0) {
    for (const auto network : networksToKill) {
      networks.Remove(network);
    }
    networksToKill.Empty();
  }

  // Tick all live networks
  for (const auto network : networks) {
    if (network) {
      network->Tick();
    }
  }

  for (const auto network : networks) {
    if (network) {
      network->EndTick();
    }
  }
}

void UResourceNetworkManager::SetSavedState(const TMap<int32, TArray<int64>> &inNetworkCharges) {
  savedNetworkCharges = inNetworkCharges;
}

void UResourceNetworkManager::ApplySavedState() {
  if (savedNetworkCharges.Num() == 0) {
    return;
  }

  for (UBlockNetwork *network : networks) {
    if (!network || network->mIsDataNetwork) {
      continue;
    }

    // Restore charge for each runtime subnetwork directly from saved Net/Sub ids
    // on its wires. One runtime subnetwork may have many wires; all of them share
    // the same saved subnetwork id, so repeated assignments are harmless.
    for (UConductorBlockLogic *wire : network->mWires) {
      if (!wire) {
        continue;
      }

      const int32 savedNetId = wire->GetSavedNetworkId();
      const int32 savedSubId = wire->GetSavedSubnetworkId();
      const int32 runtimeSub = wire->Subnetwork;

      if (savedNetId < 0 || savedSubId < 0 || runtimeSub < 0) {
        continue;
      }

      if (!network->mSubnetworks.IsValidIndex(runtimeSub)) {
        continue;
      }

      USubnetwork *sub = network->mSubnetworks[runtimeSub];
      if (!sub) {
        continue;
      }

      const TArray<int64> *savedCharges = savedNetworkCharges.Find(savedNetId);
      if (!savedCharges || !savedCharges->IsValidIndex(savedSubId)) {
        continue;
      }

      const int64 savedCharge = (*savedCharges)[savedSubId];
      const int64 capacity = sub->Capacity;
      const int64 clampedCharge = capacity >= 0 ? FMath::Clamp<int64>(savedCharge, 0, capacity) : savedCharge;
      sub->Charge = clampedCharge;
    }
  }

  savedNetworkCharges.Empty();
}
