#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SimulationSurfaceSubsystem.generated.h"

class ADimension;
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
                             ADimension *PresentationForRebind = nullptr);
  /** Drops every per-surface runtime when a save is loaded into temp (new game / reload). */
  void ResetAllSurfaceRuntimesForSaveLoad();

  void GetSurfaceFolderNames(TArray<FString> &OutNames) const;
  UDimensionRuntime *GetRuntime(const FString &SurfaceFolderName) const;
  bool HasRuntime(const FString &SurfaceFolderName) const;

  private:
  UPROPERTY()
  TMap<FString, UDimensionRuntime *> SurfaceRuntimes;
};
