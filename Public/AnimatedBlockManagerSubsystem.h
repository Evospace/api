#pragma once
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AnimatedBlockManagerSubsystem.generated.h"

class AAnimatedBlockActor;
class UBlockLogic;

UCLASS()
class UAnimatedBlockManagerSubsystem : public UTickableWorldSubsystem {
  GENERATED_BODY()

  public:
  // Global tick interval for animated blocks when far; 0 for every frame
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Budget")
  float FarTickInterval = 0.2f;

  // Distance thresholds squared (2D) for tick modes
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Budget")
  float NearDistance2 = 1500.0f * 1500.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Budget")
  float MidDistance2 = 2500.0f * 2500.0f;

  // Register/unregister
  void RegisterAnimated(AAnimatedBlockActor *Actor);
  void UnregisterAnimated(AAnimatedBlockActor *Actor);

  // UTickableWorldSubsystem
  virtual void Tick(float DeltaTime) override;
  virtual TStatId GetStatId() const override {
    RETURN_QUICK_DECLARE_CYCLE_STAT(AnimatedBlockManagerSubsystem, STATGROUP_Tickables);
  }

  private:
  struct FAnimatedEntry {
    AAnimatedBlockActor * Actor;
    float Accumulated = 0.0f;
    float Progress = 0.0f;
    float Speed = 0.0f;
  };

  TArray<FAnimatedEntry> Animated;

  float ComputeDistance2D2(const FVector &A, const FVector &B) const;
};


