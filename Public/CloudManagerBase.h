#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CloudManagerBase.generated.h"

/**
 * Base class for cloud quality manager.
 */
UCLASS(BlueprintType)
class ACloudManagerBase : public AActor {
  GENERATED_BODY()

  public:
  UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Clouds")
  void ApplyCloudSettings(int32 CloudPreset);

  virtual void ApplyCloudSettings_Implementation(int32 CloudPreset);
};

