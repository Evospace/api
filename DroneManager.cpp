#include "Public/DroneManager.h"

#include "Public/DroneStationBlockLogic.h"
#include "Evospace/Online/Actions/RailNetActions.h"
#include "Public/StationNameUtils.h"

void UDroneManager::RegisterStation(UDroneStationBlockLogic *Station) {
  if (!Station) {
    return;
  }
  if (Station->StationName.IsEmpty()) {
    Station->StationName = GenerateDefaultStationName();
  }
  Stations.Add(Station->GetBlockPos(), Station);
}

void UDroneManager::UnregisterStation(UDroneStationBlockLogic *Station) {
  if (!Station) {
    return;
  }
  if (UDroneStationBlockLogic **FoundStation = Stations.Find(Station->GetBlockPos())) {
    if (*FoundStation == Station) {
      Stations.Remove(Station->GetBlockPos());
    }
  }
}

UDroneStationBlockLogic *UDroneManager::FindStationAt(const Vec3i &Pos) const {
  UDroneStationBlockLogic *const *FoundStation = Stations.Find(Pos);
  return FoundStation ? *FoundStation : nullptr;
}

void UDroneManager::FindStationsByName(const FString &Name, TArray<UDroneStationBlockLogic *> &Out) const {
  Out.Reset();
  if (Name.IsEmpty()) {
    return;
  }
  for (const TPair<FQrVector3i, UDroneStationBlockLogic *> &Pair : Stations) {
    if (Pair.Value && Pair.Value->StationName == Name) {
      Out.Add(Pair.Value);
    }
  }
  Out.Sort([](const UDroneStationBlockLogic &A, const UDroneStationBlockLogic &B) {
    return A.GetBlockPos() < B.GetBlockPos();
  });
}

bool UDroneManager::RenameStation(APlayerController *Pc, UDroneStationBlockLogic *Station, const FString &NewName) {
  if (!Station || FindStationAt(Station->GetBlockPos()) != Station) {
    return false;
  }
  const FString Trimmed = NewName.TrimStartAndEnd();
  if (Trimmed.IsEmpty() || Trimmed == Station->StationName) {
    return false;
  }

  Station->StationName = Trimmed;

  if (Pc) {
    FStationRenameAction Action;
    const auto Pos = Station->GetBlockPos();
    Action.Pos = FIntVector(Pos.X, Pos.Y, Pos.Z);
    Action.NewName = Trimmed;
    SubmitNetAction(Pc, Action);
  }
  return true;
}

FString UDroneManager::GenerateDefaultStationName() const {
  return EvoStationName::SmallestFreeDefaultName(GetAllStationNames());
}

TArray<FString> UDroneManager::GetAllStationNames() const {
  TArray<FString> Names;
  for (const TPair<FQrVector3i, UDroneStationBlockLogic *> &Pair : Stations) {
    if (Pair.Value && !Pair.Value->StationName.IsEmpty()) {
      Names.AddUnique(Pair.Value->StationName);
    }
  }
  Names.Sort();
  return Names;
}
