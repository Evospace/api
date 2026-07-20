// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "VehicleManager.generated.h"

class ADimension;
class AVehiclePawnBase;
class UStaticItem;
class FJsonObject;

USTRUCT()
struct FPendingVehicleSpawn {
  GENERATED_BODY()

  UPROPERTY()
  UClass *PawnClass = nullptr;

  UPROPERTY()
  UStaticItem *SourceItem = nullptr;

  FTransform Transform;
};

UCLASS()
class UVehicleManager : public UObject {
  GENERATED_BODY()

  public:
  void Initialize(ADimension *Owner);

  void TickVisual(float DeltaTime);

  bool SerializeJson(TSharedPtr<FJsonObject> Json) const;
  bool DeserializeJson(TSharedPtr<FJsonObject> Json);

  void RegisterVehicle(AVehiclePawnBase *Vehicle);
  void UnregisterVehicle(AVehiclePawnBase *Vehicle);

  private:
  bool TrySpawnPending(const FPendingVehicleSpawn &Pending);

  TWeakObjectPtr<ADimension> ownerDimension;

  TArray<TWeakObjectPtr<AVehiclePawnBase>> Vehicles;

  UPROPERTY()
  TArray<FPendingVehicleSpawn> PendingSpawns;
};
