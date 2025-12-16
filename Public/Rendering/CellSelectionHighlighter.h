#pragma once

#include "CoreMinimal.h"
#include "Qr/Vector.h"

class ACellHighlightActor;

/**
 * Helper that owns a transient highlight actor for visualizing arbitrary cell selections.
 */
struct FCellSelectionHighlighter {
  void UpdateSelection(UWorld *World, const TArray<FVector3i> &Cells);
  void Shutdown();
  void SetSelectionColor(const FLinearColor &Color);

  private:
  void EnsureActor(UWorld *World);

  TWeakObjectPtr<ACellHighlightActor> HighlightActor;
  FLinearColor ActiveColor = FLinearColor(0.2f, 0.7f, 1.0f, 1.0f);
};
