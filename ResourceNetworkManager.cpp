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

  network->InitializeNetwork();
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
