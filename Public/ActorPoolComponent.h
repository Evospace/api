#pragma once

#include "CoreMinimal.h"
#include "EvoRingBuffer.h"
#include "Components/ActorComponent.h"

#include "ActorPoolComponent.generated.h"

/**
 * General-purpose actor pool backed by EvoRingBuffer.
 * Pre-spawns a fixed number of actors on BeginPlay and cycles through them.
 * GetNext resets transform and returns an already spawned actor.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class EVOSPACE_API UActorPoolComponent : public UActorComponent {
  GENERATED_BODY()

  public:
  UActorPoolComponent();

  /** Actor class to spawn for the pool. Must be set before BeginPlay. */
  UPROPERTY(EditAnywhere, Category = "Actor Pool")
  TSubclassOf<AActor> ActorClass;

  /** Number of actors to pre-spawn in the pool. Will be rounded to power of two internally by ring. */
  UPROPERTY(EditAnywhere, Category = "Actor Pool")
  int32 PoolSize = 32;

  /** Spawn all actors now (if not yet), returns number spawned. Safe to call multiple times. */
  UFUNCTION(BlueprintCallable, Category = "Actor Pool")
  int32 InitializePool();

  /** Returns next pooled actor, resetting transform. Spawns if not initialized yet. */
  UFUNCTION(BlueprintCallable, Category = "Actor Pool")
  AActor *GetNext(const FTransform &WorldTransform);

  /** Manually reset an actor's transform and optionally attach to parent. */
  UFUNCTION(BlueprintCallable, Category = "Actor Pool")
  void ResetActor(AActor *Actor, const FTransform &WorldTransform, USceneComponent *AttachParent = nullptr,
                  FName SocketName = NAME_None);

  protected:
  virtual void BeginPlay() override;

  private:
  /** Backing storage of spawned actors. We use the ring for index cycling. */
  TArray<TWeakObjectPtr<AActor>> Spawned;

  /** Ring over indices into Spawned array. */
  TUniquePtr<EvoRingBuffer<int32>> IndexRing;

  /** Next index to hand out (monotonic counter for ring addressing). */
  uint32 NextCounter = 0;

  AActor *SpawnOne();
};
