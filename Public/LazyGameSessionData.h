#pragma once
#include "CoreMinimal.h"
#include "LazyGameSessionData.generated.h"

class UGameSessionSubsystem;

USTRUCT(BlueprintType)
struct FLazyGameSession {
  GENERATED_BODY()

  public:

  UGameSessionSubsystem *Get(const UObject *WorldContextObject) const;

  UPROPERTY(EditAnywhere)
  mutable UGameSessionSubsystem *gss = nullptr;
};