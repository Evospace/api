// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "InventorySort.generated.h"

UENUM(BlueprintType)
enum class EInventorySortKey : uint8 {
  Name,
  Category,
  Tier
};

USTRUCT(BlueprintType)
struct FInventorySortRule final {
  GENERATED_BODY()

  public:
  FInventorySortRule() = default;
  FInventorySortRule(EInventorySortKey key, bool descending)
    : Key(key), Descending(descending) {}

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  EInventorySortKey Key = EInventorySortKey::Name;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool Descending = false;
};
