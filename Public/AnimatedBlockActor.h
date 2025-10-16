#pragma once
#include "CoreMinimal.h"
#include "Public/BlockActor.h"
#include "AnimatedBlockActor.generated.h"

class UAudioComponent;
class USkeletalMeshComponent;

UCLASS(BlueprintType)
class AAnimatedBlockActor : public ABlockActor {
  GENERATED_BODY()

  public:
  static void lua_reg(lua_State *L) {
    luabridge::getGlobalNamespace(L)
      .deriveClass<AAnimatedBlockActor, ABlockActor>("AnimatedBlockActor") //@class AnimatedBlockActor : BlockActor
      .endClass();
  }

  // Progress/speed tracked in manager for cache locality

  // Working speed is always 1; no per-actor override.

  // Optional built-in components for animation and audio
  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation|Components")
  USkeletalMeshComponent *FirstSkeleton = nullptr;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation|Components")
  UAudioComponent *FirstAudio = nullptr;

  // Called after manager updates speed and progress
  UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Animation")
  void OnAnimationUpdate(float DeltaTime, float Speed, float NewProgress);

  // Registration with the manager happens automatically in BeginPlay/EndPlay
  virtual void BeginPlay() override;
  virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

  // Stops animation and audio at the highest simulation LOD (manager's FarTickInterval)
  UFUNCTION(BlueprintCallable, Category = "Animation|LOD")
  void UpdateLODStopRestart(bool bAtSimulationHighestLOD);

  private:
  bool bStoppedByLOD = false;
};


