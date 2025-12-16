// Helper structures for per-miner allocation on resource sources.
#pragma once

#include "CoreMinimal.h"
#include "SourceDataExtensions.generated.h"

USTRUCT()
struct FSourceMinerAllocation {
  GENERATED_BODY()

  UPROPERTY()
  TWeakObjectPtr<UObject> Miner;

  UPROPERTY()
  int32 Speed = 0;
};


