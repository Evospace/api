#include "AnimatedBlockManagerSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "EngineUtils.h"
#include "Stats/Stats.h"
#include "Public/AnimatedBlockActor.h"
#include "Public/BlockLogic.h"

DECLARE_CYCLE_STAT(TEXT("AnimatedBlockManager.Tick"), STAT_AnimatedBlockManager_Tick, STATGROUP_UObjects);
DECLARE_CYCLE_STAT(TEXT("AnimatedBlockManager.UpdateActor"), STAT_AnimatedBlockManager_UpdateActor, STATGROUP_UObjects);

void UAnimatedBlockManagerSubsystem::RegisterAnimated(AAnimatedBlockActor *Actor) {
  if (!Actor) return;
  FAnimatedEntry Entry;
  Entry.Actor = Actor;
  Animated.Add(Entry);
}

void UAnimatedBlockManagerSubsystem::UnregisterAnimated(AAnimatedBlockActor *Actor) {
  Animated.RemoveAll([Actor](const FAnimatedEntry &E){ return E.Actor == Actor; });
}

float UAnimatedBlockManagerSubsystem::ComputeDistance2D2(const FVector &A, const FVector &B) const {
  const float DX = A.X - B.X;
  const float DY = A.Y - B.Y;
  return DX * DX + DY * DY;
}

void UAnimatedBlockManagerSubsystem::Tick(float DeltaTime) {
  if (Animated.Num() == 0) return;

  SCOPE_CYCLE_COUNTER(STAT_AnimatedBlockManager_Tick);

  // Get player viewpoint once
  const UWorld *World = GetWorld();
  if (!World) return;
  FVector ViewLoc = FVector::ZeroVector;
  if (const APlayerController *PC = World->GetFirstPlayerController()) {
    FRotator Dummy;
    PC->GetPlayerViewPoint(ViewLoc, Dummy);
  }

  for (int32 i = Animated.Num() - 1; i >= 0; --i) {
    auto &E = Animated[i];
    AAnimatedBlockActor *Actor = E.Actor;
    if (!Actor) {
      Animated.RemoveAtSwap(i);
      continue;
    }

    const FVector ActorLoc = Actor->GetActorLocation();
    const float Dist2 = ComputeDistance2D2(ViewLoc, ActorLoc);

    // Choose tick cadence based on distance
    float LocalTickInterval = 0.0f; // frame
    const bool bFarLOD = Dist2 > MidDistance2; // when true, we use FarTickInterval
    if (bFarLOD) {
      LocalTickInterval = FarTickInterval; // sparse updates (highest simulation LOD)
    } else if (Dist2 > NearDistance2) {
      LocalTickInterval = 0.05f; // mid rate
    }

    E.Accumulated += DeltaTime;
    if (LocalTickInterval > 0.f && (E.Accumulated + KINDA_SMALL_NUMBER) < LocalTickInterval) {
      continue;
    }
    const float StepDelta = LocalTickInterval > 0.f ? LocalTickInterval : DeltaTime;
    E.Accumulated = 0.0f;

    {
      SCOPE_CYCLE_COUNTER(STAT_AnimatedBlockManager_UpdateActor);

      const bool bWorking = Actor->mBlockLogic->IsWorking();
      const auto alpha = FMath::Clamp(StepDelta, 0.0f, 1.0f);
      E.Speed = FMath::Lerp(E.Speed, bWorking ? 1.0f : 0.0f, alpha);
      E.Progress += E.Speed * StepDelta;
      
      Actor->UpdateLODStopRestart(bFarLOD);
      Actor->OnAnimationUpdate(StepDelta, E.Speed, E.Progress);
    }
  }
}


