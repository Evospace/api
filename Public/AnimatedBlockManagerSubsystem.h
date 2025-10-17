#pragma once
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "UObject/WeakObjectPtr.h"
#include "Containers/Array.h"
#include "AnimatedBlockManagerSubsystem.generated.h"

class AAnimatedBlockActor;
class UBlockLogic;

USTRUCT(BlueprintType)
struct FLodLevel
{
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Budget")
  float Distance2 = 0.0f; // apply when Distance2 > this

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Budget")
  float Interval = 0.0f; // 0 == every frame

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Budget")
  bool IsSfxDisabled = false;
};

UCLASS()
class UAnimatedBlockManagerSubsystem : public UTickableWorldSubsystem {
  GENERATED_BODY()

  public:
  // LOD table (sorted ascending by Distance2). Initialized in Initialize().
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation|Budget")
  TArray<FLodLevel> LodLevels;

  // Debug visualization controls
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Debug")
  bool bVisualizeActorTicks = false;

  // World units per single "block" for debug height conversion
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Debug", meta=(ClampMin="1.0"))
  float DebugBlockUnitUU = 100.0f;

  // Debug line thickness in pixels
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Debug", meta=(ClampMin="0.1", ClampMax="20.0"))
  float DebugLineThickness = 2.0f;

  // Register/unregister
  void RegisterAnimated(AAnimatedBlockActor *Actor);
  void UnregisterAnimated(AAnimatedBlockActor *Actor);

  // UTickableWorldSubsystem
  virtual void Initialize(FSubsystemCollectionBase& Collection) override;
  virtual void Tick(float DeltaTime) override;
  virtual void Deinitialize() override;
  virtual TStatId GetStatId() const override {
    RETURN_QUICK_DECLARE_CYCLE_STAT(AnimatedBlockManagerSubsystem, STATGROUP_Tickables);
  }

  private:
  struct FAnimatedEntry {
    TWeakObjectPtr<AAnimatedBlockActor> Actor;
    float Accumulated = 0.0f;
    float Progress = 0.0f;
    float Speed = 0.0f;
  };

  TArray<FAnimatedEntry> Animated;
  TMap<AAnimatedBlockActor*, int32> ActorToIndex;

  bool bManagerShuttingDown = false;

  float ComputeDistance2D2(const FVector &A, const FVector &B) const;
};


