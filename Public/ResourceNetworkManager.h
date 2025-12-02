// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "ResourceNetworkManager.generated.h"

class ADimension;
class UBlockNetwork;

UCLASS()
class UResourceNetworkManager : public UObject {
  GENERATED_BODY()

  public:
  /** Bind this manager to its owning Dimension. Must be called once after construction. */
  void Initialize(ADimension *inOwner);

  /** Create a new resource network owned by this manager. */
  UFUNCTION(BlueprintCallable)
  UBlockNetwork *CreateNetwork(bool bIsDataNetwork);

  /** Mark network for deferred destruction at safe point. */
  UFUNCTION(BlueprintCallable)
  void KillNetworkDeferred(UBlockNetwork *network);

  /** Per-tick update for all managed networks (called from Dimension tick). */
  void Tick();

  /** Read-only enumeration helper for diagnostics/testing. */
  const TSet<UBlockNetwork *> &GetNetworks() const { return networks; }

  /** Provide previously saved per-network charges so they can be applied after rebuild. */
  void SetSavedState(const TMap<int32, TArray<int64>> &inNetworkCharges);

  /** Apply saved charges to rebuilt networks based on conductor mappings. */
  void ApplySavedState();

  private:
  /** Back-reference to owning Dimension, kept weak to avoid cycles. */
  UPROPERTY()
  TWeakObjectPtr<ADimension> ownerDimension;

  /** All alive block networks for this Dimension. */
  UPROPERTY()
  TSet<UBlockNetwork *> networks;

  /** Networks scheduled for removal. */
  UPROPERTY()
  TSet<UBlockNetwork *> networksToKill;

  /** Saved subnetwork charges keyed by transient NetworkId from save file. */
  TMap<int32, TArray<int64>> savedNetworkCharges;
};


