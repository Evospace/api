#include "ActorPoolComponent.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"

UActorPoolComponent::UActorPoolComponent() {
  PrimaryComponentTick.bCanEverTick = false;
}

void UActorPoolComponent::BeginPlay() {
  Super::BeginPlay();
  InitializePool();
}

int32 UActorPoolComponent::InitializePool() {
  if (!ActorClass)
    return 0;
  if (PoolSize <= 0)
    PoolSize = 1;

  // If already initialized for this size, do nothing
  if (Spawned.Num() >= PoolSize && IndexRing.IsValid())
    return Spawned.Num();

  Spawned.Reserve(PoolSize);
  for (int32 i = Spawned.Num(); i < PoolSize; ++i) {
    AActor *A = SpawnOne();
    Spawned.Add(A);
  }

  IndexRing = MakeUnique<EvoRingBuffer<int32>>(FMath::Max(1, PoolSize));
  // Seed ring values to map ring capacity to pool indices modulo PoolSize
  const int32 ringCapacity = static_cast<int32>(IndexRing->Capacity());
  for (int32 idx = 0; idx < ringCapacity; ++idx) {
    IndexRing->Set(static_cast<uint32>(idx), idx % PoolSize);
  }
  NextCounter = 0;

  return Spawned.Num();
}

AActor *UActorPoolComponent::SpawnOne() {
  UWorld *World = GetWorld();
  if (!World || !ActorClass)
    return nullptr;

  FActorSpawnParameters Params;
  Params.Owner = GetOwner();
  Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
  return World->SpawnActor<AActor>(ActorClass, FTransform::Identity, Params);
}

void UActorPoolComponent::ResetActor(AActor *Actor, const FTransform &WorldTransform, USceneComponent *AttachParent,
                                     FName SocketName) {
  if (!Actor)
    return;
  if (AttachParent) {
    Actor->AttachToComponent(AttachParent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
  } else {
    Actor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
  }
  Actor->SetActorTransform(WorldTransform);
  Actor->SetActorHiddenInGame(false);
  Actor->SetActorEnableCollision(true);
  Actor->SetActorTickEnabled(true);
}

AActor *UActorPoolComponent::GetNext(const FTransform &WorldTransform) {
  if (!ActorClass)
    return nullptr;
  if (!IndexRing.IsValid() || Spawned.Num() < PoolSize) {
    InitializePool();
  }
  if (!IndexRing.IsValid() || Spawned.Num() == 0)
    return nullptr;

  const uint32 ringIndex = NextCounter++;
  const int32 pooledIndex = (*IndexRing)[ringIndex];
  AActor *Actor = Spawned.IsValidIndex(pooledIndex) ? Spawned[pooledIndex].Get() : nullptr;

  if (!IsValid(Actor)) {
    // Respawn missing actor in place
    LOG(ERROR_LL) << "ActorPoolComponent::GetNext: Actor is invalid";
    Actor = SpawnOne();
    if (Spawned.IsValidIndex(pooledIndex)) {
      Spawned[pooledIndex] = Actor;
    }
  }

  ResetActor(Actor, WorldTransform);
  return Actor;
}
