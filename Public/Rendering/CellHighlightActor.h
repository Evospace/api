#pragma once

#include "CoreMinimal.h"
#include "Containers/BitArray.h"
#include "GameFramework/Actor.h"
#include "Qr/Vector.h"

#include "CellHighlightActor.generated.h"
class UInstancedStaticMeshComponent;

/**
 * Lightweight actor that visualizes cell-sized previews using instanced meshes.
 *
 * Can work in two modes:
 *   - Relative: cell offsets are applied to owning actor/world transform.
 *   - Absolute: explicit world-block coordinates are provided.
 *
 * All rendered cells share the same highlight color.
 */
UCLASS(BlueprintType)
class ACellHighlightActor : public AActor {
  GENERATED_BODY()

  public:
  ACellHighlightActor();

  UFUNCTION(BlueprintCallable, Category = "Cell Highlight")
  void SetRelativeCells(const TArray<FVector3i> &Cells);

  UFUNCTION(BlueprintCallable, Category = "Cell Highlight")
  void SetAbsoluteCells(const TArray<FVector3i> &Cells);

  UFUNCTION(BlueprintCallable, Category = "Cell Highlight")
  void UseAbsoluteCoordinates(bool bEnable);

  UFUNCTION(BlueprintCallable, Category = "Cell Highlight")
  void SetHighlightColor(const FLinearColor &Color);

  UFUNCTION(BlueprintCallable, Category = "Cell Highlight")
  void SetInstanceColor(int32 Index, const FLinearColor &Color);

  UFUNCTION(BlueprintCallable, Category = "Cell Highlight")
  void SetInstanceColors(const TArray<FLinearColor> &Colors);

  UFUNCTION(BlueprintPure, Category = "Cell Highlight")
  TArray<FLinearColor> GetInstanceColors() const;

  UFUNCTION(BlueprintCallable, Category = "Cell Highlight")
  void ForceRebuild();

  protected:
  virtual void Tick(float DeltaSeconds) override;

  private:
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cell Highlight", meta = (AllowPrivateAccess = "true"))
  UInstancedStaticMeshComponent *CellInstances;

  TArray<FVector3i> RelativeCells;
  TArray<FVector3i> AbsoluteCells;
  bool UseAbsoluteCoordinatesFlag = false;
  FLinearColor HighlightColor = FLinearColor(0.0f, 1.0f, 0.0f, 1.0f);

  bool NeedsFullRebuild = true;
  TArray<FLinearColor> InstanceColorOverrides;
  TBitArray<> InstanceColorOverrideMask;
  TArray<FLinearColor> CachedInstanceColors;

  void RebuildCells();
  FLinearColor ResolveInstanceColor(int32 Index) const;
};

