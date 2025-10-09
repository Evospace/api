// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Public/BlockLogic.h"

#include "TieredBlockLogic.generated.h"

class UInventoryReader;

UCLASS(BlueprintType)
/**
 *
 */
class UTieredBlockHelper : public UObject {
  using Self = UTieredBlockHelper;
  GENERATED_BODY()

  public:
  UTieredBlockHelper();

  UFUNCTION(BlueprintCallable)
  static FString TierToString(int32 t);

  UFUNCTION(BlueprintCallable)
  static FColor TierToColor(int32 t);
};

UCLASS(BlueprintType)
class UFilteringBlockLogic : public UBlockLogic {
  GENERATED_BODY()

  public:
  UFUNCTION(BlueprintCallable)
  virtual void ChangeFilter(int32 inventory, int32 slot, UStaticItem *item) {}

  UFUNCTION(BlueprintCallable)
  virtual UInventory *GetOwnInventory() { return nullptr; }
};
