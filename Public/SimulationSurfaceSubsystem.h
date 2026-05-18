#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SimulationSurfaceSubsystem.generated.h"

class ADimension;
class UInstancedStaticMeshComponent;
class UDimensionRuntime;

/** Per-surface simulation runtime handling when (re)initializing presentation or loading a save. */
UENUM()
enum class ESurfaceSimulationLifecycle : uint8 {
  /** No-op (explicit opt-out). */
  Preserve,
  /** Clear logic and managers; PresentationForRebind recreates managers when bound. Used before LoadDimentionFolder. */
  ResetBeforeLoad,
  /** Remove runtime from the subsystem map (save load, surface switch away, EndPlay). */
  Destroy,
};

UCLASS()
class EVOSPACE_API USimulationSurfaceSubsystem : public UGameInstanceSubsystem {
  GENERATED_BODY()

  public:
  virtual void Deinitialize() override;
  UDimensionRuntime *GetOrCreateRuntime(const FString &SurfaceFolderName);

  void ApplySurfaceLifecycle(const FString &SurfaceFolderName, ESurfaceSimulationLifecycle Lifecycle,
                             ADimension *PresentationForRebind = nullptr,
                             UInstancedStaticMeshComponent *DroneMeshComponent = nullptr);
  /** Drops every per-surface runtime when a save is loaded into temp (new game / reload). */
  void ResetAllSurfaceRuntimesForSaveLoad();

  private:
  UPROPERTY()
  TMap<FString, UDimensionRuntime *> SurfaceRuntimes;
};
