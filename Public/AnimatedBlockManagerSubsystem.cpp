#include "AnimatedBlockManagerSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "EngineUtils.h"
#include "Stats/Stats.h"
#include "Public/AnimatedBlockActor.h"
#include "Public/BlockLogic.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

void UAnimatedBlockManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection) {
  Super::Initialize(Collection);
  LodLevels.Reset();
  // Hardcoded three LODs. Distances are squared; tune as needed.
  // LOD1 (mid): > 1500uu, interval 0.05s
  LodLevels.Add({ 2000.0f * 2000.0f, 0.05f, false });
  // LOD2 (far): > 2500uu, interval 0.2s
  LodLevels.Add({ 3500.0f * 3500.0f, 0.2f, true });
  // LOD3 (very far): > 4000uu, interval 0.5s, highest sim LOD
  LodLevels.Add({ 5500.0f * 5500.0f, 0.9f, true });
}


DECLARE_CYCLE_STAT(TEXT("AnimatedBlockManager.Tick"), STAT_AnimatedBlockManager_Tick, STATGROUP_UObjects);
DECLARE_CYCLE_STAT(TEXT("AnimatedBlockManager.UpdateActor"), STAT_AnimatedBlockManager_UpdateActor, STATGROUP_UObjects);

void UAnimatedBlockManagerSubsystem::RegisterAnimated(AAnimatedBlockActor *Actor) {
  if (!Actor || bManagerShuttingDown) return;
  if (ActorToIndex.Contains(Actor)) return; // prevent duplicates
  const int32 NewIndex = Animated.Add(FAnimatedEntry{Actor});
  ActorToIndex.Add(Actor, NewIndex);
}

void UAnimatedBlockManagerSubsystem::Deinitialize() {
  bManagerShuttingDown = true;
  ActorToIndex.Reset();
  Animated.Reset();
  Super::Deinitialize();
}

void UAnimatedBlockManagerSubsystem::UnregisterAnimated(AAnimatedBlockActor *Actor) {
  if (!Actor) return;
  int32 Index = INDEX_NONE;
  if (!ActorToIndex.RemoveAndCopyValue(Actor, Index)) return;
  if (!Animated.IsValidIndex(Index)) return;
  Animated.RemoveAtSwap(Index);
  // Fix moved element index
  if (Animated.IsValidIndex(Index)) {
    if (AAnimatedBlockActor* MovedActor = Animated[Index].Actor.Get()) {
      if (int32* Entry = ActorToIndex.Find(MovedActor)) {
        *Entry = Index;
      }
    }
  }
}

float UAnimatedBlockManagerSubsystem::ComputeDistance2D2(const FVector &A, const FVector &B) const {
  const float DX = A.X - B.X;
  const float DY = A.Y - B.Y;
  return DX * DX + DY * DY;
}

// removed SelectLodInterval

void UAnimatedBlockManagerSubsystem::Tick(float DeltaTime) {
  if (bManagerShuttingDown) return;
  if (Animated.Num() == 0) return;

  SCOPE_CYCLE_COUNTER(STAT_AnimatedBlockManager_Tick);

  // Get player viewpoint once
  UWorld *World = GetWorld();
  if (!World || World->bIsTearingDown) return;
  FVector ViewLoc = FVector::ZeroVector;
  if (const APlayerController *PC = World->GetFirstPlayerController()) {
    FRotator Dummy;
    PC->GetPlayerViewPoint(ViewLoc, Dummy);
  }

  for (int32 i = Animated.Num() - 1; i >= 0; --i) {
    auto &E = Animated[i];
    AAnimatedBlockActor *Actor = E.Actor.Get();
    if (!Actor || Actor->IsActorBeingDestroyed()) {
      // fast remove via map
      int32 Index;
      if (Actor && ActorToIndex.RemoveAndCopyValue(Actor, Index)) {
        Animated.RemoveAtSwap(Index);
        if (Animated.IsValidIndex(Index)) {
          if (AAnimatedBlockActor* MovedActor = Animated[Index].Actor.Get()) {
            if (int32* Entry = ActorToIndex.Find(MovedActor)) {
              *Entry = Index;
            }
          }
        }
      } else {
        Animated.RemoveAtSwap(i);
        if (Animated.IsValidIndex(i)) {
          if (AAnimatedBlockActor* MovedActor = Animated[i].Actor.Get()) {
            if (int32* Entry = ActorToIndex.Find(MovedActor)) {
              *Entry = i;
            }
          }
        }
      }
      continue;
    }

    const FVector ActorLoc = Actor->GetActorLocation();
    const float Dist2 = ComputeDistance2D2(ViewLoc, ActorLoc);

    // Smooth cadence: 0s up to 2000^2, linearly to 1.0s at 6000^2
    const float MinD2 = 2000.0f * 2000.0f;
    const float MaxD2 = 6000.0f * 6000.0f;
    float LocalTickInterval = 0.0f;
    if (Dist2 <= MinD2) {
      LocalTickInterval = 0.0f;
    } else if (Dist2 >= MaxD2) {
      LocalTickInterval = 1.0f;
    } else {
      const float t = (Dist2 - MinD2) / (MaxD2 - MinD2);
      LocalTickInterval = FMath::Clamp(t, 0.0f, 1.0f);
    }
    // SFX disabling threshold (keep conservative)
    const float SfxOffD2 = 3500.0f * 3500.0f;
    const bool IsSfxDisabled = Dist2 >= SfxOffD2;

    E.Accumulated += DeltaTime;
    if (LocalTickInterval > 0.f && (E.Accumulated + KINDA_SMALL_NUMBER) < LocalTickInterval) {
      continue;
    }
    const float StepDelta = LocalTickInterval > 0.f ? LocalTickInterval : DeltaTime;
    E.Accumulated = 0.0f;

    {
      SCOPE_CYCLE_COUNTER(STAT_AnimatedBlockManager_UpdateActor);

      bool bWorking = false;
      // can be null in building holo
      if (Actor->mBlockLogic) {
        bWorking = Actor->mBlockLogic->IsWorking();
      }
      const auto alpha = FMath::Clamp(StepDelta, 0.0f, 1.0f);
      E.Speed = FMath::Lerp(E.Speed, bWorking ? 1.0f : 0.0f, alpha);
      E.Progress += E.Speed * StepDelta;
      
      Actor->UpdateLODStopRestart(IsSfxDisabled);
      Actor->OnAnimationUpdate(StepDelta, E.Speed, E.Progress);

#if WITH_EDITOR
      if (bVisualizeActorTicks) {
        const FVector Start = ActorLoc;
        const float HeightUU = DebugBlockUnitUU * 10.0f;
        const FVector End = Start + FVector(0.0f, 0.0f, HeightUU);

        FColor Color = FColor::Yellow;
        if (IsSfxDisabled) {
          Color = FColor::Green; // components are disabled beyond SfxOffD2
        } else {
          int32 numParts = 0;
          if (Actor->FirstSkeleton) ++numParts;
          if (Actor->FirstParticle) ++numParts;
          const float t = FMath::Clamp(static_cast<float>(numParts) / 2.0f, 0.0f, 1.0f);
          const uint8 g = static_cast<uint8>(FMath::RoundToInt(255.0f * (1.0f - t)));
          Color = FColor(255, g, 0);
        }

        DrawDebugLine(World, Start, End, Color, /*bPersistentLines*/ false, /*LifeTime*/ 0.0f, /*DepthPriority*/ 0, /*Thickness*/ DebugLineThickness);
      }
#endif
    }
  }
}


