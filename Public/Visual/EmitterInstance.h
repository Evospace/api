#pragma once
#include "EmitterInstance.generated.h"

class UNiagaraSystem;
class UBlockLogic;
class UNiagaraComponent;
class ASector;

USTRUCT()
struct FEmitterInstance {
  GENERATED_BODY()

  UPROPERTY()
  UNiagaraComponent *FX = nullptr;

  UPROPERTY()
  UBlockLogic *Block = nullptr;

  float LastValue = -1.f;
};

UCLASS(ClassGroup = Visual, meta = (BlueprintSpawnableComponent))
class UEmitterBatchComponent : public UActorComponent {
  GENERATED_BODY()
  public:
  void AddEmitter(UBlockLogic *Block, UNiagaraSystem *system, const FTransform &Relative);

  virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

  virtual void BeginPlay() override;

  UPROPERTY()
  TArray<FEmitterInstance> Instances;
};