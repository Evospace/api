#pragma once

#include "CoreMinimal.h"
#include "FactoryVisualSubsystem.generated.h"

class UEmitterBatchComponent;

UCLASS()
class UFactoryVisualSubsystem : public UTickableWorldSubsystem {
  GENERATED_BODY()
  public:
  virtual void Tick(float DeltaTime) override;

  virtual void Initialize(FSubsystemCollectionBase &Collection) override;

  virtual TStatId GetStatId() const override;

  private:
  float collector = 0;

  TSet<UEmitterBatchComponent *> EmitterBatches;
};