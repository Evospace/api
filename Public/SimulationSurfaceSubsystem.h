#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SimulationSurfaceSubsystem.generated.h"

class UDimensionRuntime;

UCLASS()
class EVOSPACE_API USimulationSurfaceSubsystem : public UGameInstanceSubsystem {
  GENERATED_BODY()

  public:
  virtual void Deinitialize() override;
  UDimensionRuntime *GetOrCreateRuntime(const FString &SurfaceFolderName);

  private:
  UPROPERTY()
  TMap<FString, UDimensionRuntime *> SurfaceRuntimes;
};
