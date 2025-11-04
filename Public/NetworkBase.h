// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"

#include "NetworkBase.generated.h"

UCLASS(Abstract)
class UNetworkBase : public UObject {
  GENERATED_BODY()
  public:
  virtual void Tick() {}
  virtual void EndTick() {}

  UFUNCTION(BlueprintCallable)
  void MarkDirty() { bCollectedDirty = true; }

  UFUNCTION(BlueprintCallable)
  void DeferredKill() { bKillDeferred = true; }

  UFUNCTION(BlueprintCallable)
  bool IsKillDeferred() const { return bKillDeferred; }

  protected:
  UPROPERTY(VisibleAnywhere)
  bool bCollectedDirty = true;

  UPROPERTY(VisibleAnywhere)
  bool bKillDeferred = false;

  UPROPERTY(VisibleAnywhere)
  bool bUpdated = false;
};


