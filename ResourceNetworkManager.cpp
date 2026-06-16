// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#include "Public/ResourceNetworkManager.h"

#include "Public/Dimension.h"
#include "Public/DimensionRuntime.h"
#include "Public/ConductorBlockLogic.h"
#include "Evospace/JsonHelper.h"
#include "Evospace/Misc/GraphStorage.h"
#include "Qr/StaticLogger.h"

#include "HAL/IConsoleManager.h"

namespace {
static TAutoConsoleVariable<int32> CVarResourceNetworkSimulate(
  TEXT("evospace.ResourceNetwork.Simulate"),
  1,
  TEXT("1: run resource/data network simulation each tick. 0: skip network Tick/EndTick (bookkeeping only)."),
  ECVF_Default);

bool Vec3iLess(const Vec3i &a, const Vec3i &b) {
  if (a.X != b.X) return a.X < b.X;
  if (a.Y != b.Y) return a.Y < b.Y;
  return a.Z < b.Z;
}

TArray<Vec3i> CollectSortedWirePositions(const UBlockNetwork *network) {
  TArray<Vec3i> positions;
  if (!network) {
    return positions;
  }

  positions.Reserve(network->mWires.Num());
  for (UConductorBlockLogic *wire : network->mWires) {
    if (wire) {
      positions.Add(wire->GetBlockPos());
    }
  }

  positions.Sort(Vec3iLess);

  return positions;
}

bool WireSetsEqual(const TArray<Vec3i> &a, const TArray<Vec3i> &b) {
  if (a.Num() != b.Num()) {
    return false;
  }
  if (a.Num() == 0) {
    return true;
  }
  return FMemory::Memcmp(a.GetData(), b.GetData(), a.Num() * sizeof(Vec3i)) == 0;
}

bool HasGraphData(const UGraphStorage *storage) {
  return storage && storage->mGraphMap.Num() > 0;
}

void ApplyGraphStorageJson(UGraphStorage *target, const TSharedPtr<FJsonObject> &json) {
  if (!target || !json.IsValid()) {
    return;
  }

  UGraphStorage *temp = NewObject<UGraphStorage>(target);
  temp->Initialize();
  temp->DeserializeJson(json);
  target->CopyFrom(temp);
}

} // namespace

bool UResourceNetworkManager::IsSimulationEnabled() {
  return CVarResourceNetworkSimulate.GetValueOnGameThread() != 0;
}

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
  if (!network->IsKillDeferred()) {
    networksToKill.Add(network);
    network->DeferredKill();
  }
}

void UResourceNetworkManager::Tick() {

  // Called by Dimension before block logic ticks. Data networks aggregate outputs and
  // apply to inputs here; blocks then read that state in their Tick() (one-tick delay).
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

  if (!IsSimulationEnabled()) {
    return;
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

bool UResourceNetworkManager::SerializeJson(TSharedPtr<FJsonObject> json) const {
  if (!json.IsValid()) {
    return false;
  }

  TArray<TSharedPtr<FJsonValue>> networkEntries;
  for (UBlockNetwork *network : networks) {
    if (!network || network->IsEmpty()) {
      continue;
    }
    if (!HasGraphData(network->mProductionStorage) && !HasGraphData(network->mConsumptionStorage)) {
      continue;
    }

    const TArray<Vec3i> wires = CollectSortedWirePositions(network);
    if (wires.Num() == 0) {
      continue;
    }

    TSharedPtr<FJsonObject> networkJson = MakeShareable(new FJsonObject());
    json_helper::TrySet(networkJson, TEXT("Wires"), wires);
    json_helper::TrySet(networkJson, TEXT("IsDataNetwork"), network->mIsDataNetwork);

    if (HasGraphData(network->mProductionStorage)) {
      TSharedPtr<FJsonObject> productionJson = MakeShareable(new FJsonObject());
      network->mProductionStorage->SerializeJson(productionJson);
      networkJson->SetObjectField(TEXT("Production"), productionJson);
    }

    if (HasGraphData(network->mConsumptionStorage)) {
      TSharedPtr<FJsonObject> consumptionJson = MakeShareable(new FJsonObject());
      network->mConsumptionStorage->SerializeJson(consumptionJson);
      networkJson->SetObjectField(TEXT("Consumption"), consumptionJson);
    }

    networkEntries.Add(MakeShareable(new FJsonValueObject(networkJson)));
  }

  json->SetArrayField(TEXT("Networks"), networkEntries);
  return true;
}

void UResourceNetworkManager::RestoreGraphDataFromSave(TSharedPtr<FJsonObject> json) {
  if (!json.IsValid()) {
    return;
  }

  const TArray<TSharedPtr<FJsonValue>> *networkEntries = nullptr;
  if (!json->TryGetArrayField(TEXT("Networks"), networkEntries) || !networkEntries) {
    return;
  }

  int32 restoredCount = 0;
  int32 missedCount = 0;

  for (const TSharedPtr<FJsonValue> &entryValue : *networkEntries) {
    TSharedPtr<FJsonObject> entryJson = entryValue.IsValid() ? entryValue->AsObject() : nullptr;
    if (!entryJson.IsValid()) {
      continue;
    }

    TArray<Vec3i> savedWires;
    if (!json_helper::TryGet(entryJson, TEXT("Wires"), savedWires)) {
      LOG(WARN_LL) << "LoadSave: ResourceNetwork entry missing Wires";
      continue;
    }

    savedWires.Sort(Vec3iLess);

    UBlockNetwork *matchedNetwork = nullptr;
    for (UBlockNetwork *network : networks) {
      if (!network) {
        continue;
      }
      if (WireSetsEqual(CollectSortedWirePositions(network), savedWires)) {
        matchedNetwork = network;
        break;
      }
    }

    if (!matchedNetwork) {
      ++missedCount;
      LOG(WARN_LL) << "LoadSave: ResourceNetwork graph not matched for " << savedWires.Num() << " wires";
      continue;
    }

    TSharedPtr<FJsonObject> productionJson;
    if (json_helper::TryGet(entryJson, TEXT("Production"), productionJson)) {
      ApplyGraphStorageJson(matchedNetwork->mProductionStorage, productionJson);
    }

    TSharedPtr<FJsonObject> consumptionJson;
    if (json_helper::TryGet(entryJson, TEXT("Consumption"), consumptionJson)) {
      ApplyGraphStorageJson(matchedNetwork->mConsumptionStorage, consumptionJson);
    }

    ++restoredCount;
  }

  if (restoredCount > 0 || missedCount > 0) {
    LOG(INFO_LL) << "LoadSave: ResourceNetwork graphs restored=" << restoredCount << " missed=" << missedCount;
  }
}

void UResourceNetworkManager::RebuildAllNetworksAfterLoad(UDimensionRuntime *runtime) {
  if (!runtime) {
    return;
  }

  for (const auto &pair : runtime->GetLogics()) {
    UConductorBlockLogic *conductor = Cast<UConductorBlockLogic>(pair.Value);
    if (conductor) {
      conductor->RebuildNetworkIfDetached();
    }
  }
}
