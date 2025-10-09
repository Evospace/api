#include "EmitterInstance.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Evospace/World/Sector.h"

void UEmitterBatchComponent::AddEmitter(UBlockLogic *Block, UNiagaraSystem *system, const FTransform &Relative) {
  if (!Block || !system) {
    LOG(ERROR_LL) << "EmitterBatchComponent::AddEmitter: Block or System is null";
    return;
  }

  AActor *Sector = GetOwner();
  if (!Sector || !Sector->GetRootComponent()) {
    LOG(ERROR_LL) << "EmitterBatchComponent::AddEmitter: Component has no owning sector or root";
    return;
  }

  UNiagaraComponent *FX =
    UNiagaraFunctionLibrary::SpawnSystemAttached(system, // Niagara asset
                                                 Sector->GetRootComponent(), // Parent – корень сектора
                                                 NAME_None, // Socket name
                                                 Relative.GetLocation(), // Rel-pos
                                                 Relative.Rotator(), // Rel-rot
                                                 Relative.GetScale3D(), // Rel-scale
                                                 EAttachLocation::KeepRelativeOffset, // Transform rule
                                                 true,
                                                 ENCPoolMethod::None,
                                                 true,
                                                 true);

  if (!FX) {
    LOG(ERROR_LL) << "EmitterBatchComponent::AddEmitter: Failed to spawn NiagaraComponent";
  }

  FX->SetUsingAbsoluteLocation(false);
  FX->SetUsingAbsoluteRotation(false);
  FX->SetUsingAbsoluteScale(false);

  FEmitterInstance &Instance = Instances.Emplace_GetRef();
  Instance.FX = FX;
  Instance.Block = Block;
  Instance.LastValue = -1.f;
}

void UEmitterBatchComponent::EndPlay(const EEndPlayReason::Type EndPlayReason) {
  Super::EndPlay(EndPlayReason);
  for (auto &i : Instances) {
    if (i.FX) {
      i.FX->DestroyComponent();
    }
  }
}

void UEmitterBatchComponent::BeginPlay() {
  Super::BeginPlay();
}