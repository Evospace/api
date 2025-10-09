#pragma once
#include "CoreMinimal.h"

#include "MovementProvider.generated.h"

UINTERFACE(NotBlueprintable)
class UMovementProvider : public UInterface {
  GENERATED_BODY()
};

class IMovementProvider {
  GENERATED_BODY()

  public:
  UFUNCTION(BlueprintCallable)
  virtual void EnableMovement() PURE_VIRTUAL(UMovementProvider::EnableMovement, );

  UFUNCTION(BlueprintCallable)
  virtual void DisableMovement() PURE_VIRTUAL(UMovementProvider::DisableMovement, );
};