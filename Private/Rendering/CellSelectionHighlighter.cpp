#include "Evospace/Shared/Public/Rendering/CellSelectionHighlighter.h"

#include "Evospace/Shared/Public/Rendering/CellHighlightActor.h"
#include "Engine/World.h"

namespace {

void DestroyActor(TWeakObjectPtr<ACellHighlightActor> &ActorPtr) {
  if (ACellHighlightActor *Actor = ActorPtr.Get()) {
    Actor->Destroy();
  }
  ActorPtr = nullptr;
}

} // namespace

void FCellSelectionHighlighter::UpdateSelection(UWorld *World, const TArray<FVector3i> &Cells) {
  if (Cells.IsEmpty()) {
    if (ACellHighlightActor *Actor = HighlightActor.Get()) {
      Actor->SetAbsoluteCells({});
      Actor->SetActorHiddenInGame(true);
    }
    return;
  }

  EnsureActor(World);
  ACellHighlightActor *Actor = HighlightActor.Get();
  if (!Actor) {
    return;
  }

  Actor->SetHighlightColor(ActiveColor);
  Actor->UseAbsoluteCoordinates(true);
  Actor->SetAbsoluteCells(Cells);
  Actor->SetActorHiddenInGame(false);
}

void FCellSelectionHighlighter::Shutdown() {
  DestroyActor(HighlightActor);
}

void FCellSelectionHighlighter::SetSelectionColor(const FLinearColor &Color) {
  ActiveColor = Color;
  if (ACellHighlightActor *Actor = HighlightActor.Get()) {
    Actor->SetHighlightColor(ActiveColor);
  }
}

void FCellSelectionHighlighter::EnsureActor(UWorld *World) {
  if (HighlightActor.IsValid()) {
    if (!World || HighlightActor->GetWorld() != World) {
      DestroyActor(HighlightActor);
    } else {
      return;
    }
  }

  if (!World) {
    return;
  }

  FActorSpawnParameters params;
  params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
  params.ObjectFlags |= RF_Transient;

  ACellHighlightActor *Actor = World->SpawnActor<ACellHighlightActor>(params);
  if (!Actor) {
    return;
  }

  HighlightActor = Actor;
  Actor->UseAbsoluteCoordinates(true);
  Actor->SetHighlightColor(ActiveColor);
  Actor->SetActorHiddenInGame(true);
}


