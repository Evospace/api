#include "FactoryVisualSubsystem.h"

#include "EmitterInstance.h"
#include "Public/BlockLogic.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

void UFactoryVisualSubsystem::Tick(float DeltaTime) {
  Super::Tick(DeltaTime);
  collector += DeltaTime;

  if (collector > 0.5) {
    collector -= 0.5;

    for (UEmitterBatchComponent *Batch : EmitterBatches) {
      if (!Batch)
        continue;

      for (FEmitterInstance &instance : Batch->Instances) {
        const float loadRatio = instance.Block ? instance.Block->LoadRatio : 0.f;

        if (FMath::Abs(loadRatio - instance.LastValue) < 0.01f)
          continue;

        instance.FX->SetVariableFloat(TEXT("LoadRatioTarget"), loadRatio);
        instance.LastValue = loadRatio;
      }
    }
  }
}

void UFactoryVisualSubsystem::Initialize(FSubsystemCollectionBase &Collection) {
  Super::Initialize(Collection);
}

TStatId UFactoryVisualSubsystem::GetStatId() const {
  RETURN_QUICK_DECLARE_CYCLE_STAT(FactoryVisualSubsystem, STATGROUP_Tickables)
}