#pragma once

#include "CoreMinimal.h"
#include "UniversalSearchResult.generated.h"

USTRUCT(BlueprintType)
struct FUniversalSearchResult {
  GENERATED_BODY()

  UPROPERTY(BlueprintReadOnly)
  FString result;

  UPROPERTY(BlueprintReadOnly)
  const class UStaticResearch *research = nullptr;

  UPROPERTY(BlueprintReadOnly)
  const class UStaticTip *tip = nullptr;

  UPROPERTY(BlueprintReadOnly)
  class UInventory *items = nullptr;
};