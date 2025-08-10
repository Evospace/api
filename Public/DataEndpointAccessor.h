#pragma once
#include "CoreMinimal.h"
#include "Public/ResourceAccessor.h"
#include "DataEndpointAccessor.generated.h"

UCLASS()
class EVOSPACE_API UDataEndpointAccessor : public UResourceAccessor {
  GENERATED_BODY()
public:
  UDataEndpointAccessor() {
    Channel = UResourceAccessor::Data();
  }
}; 