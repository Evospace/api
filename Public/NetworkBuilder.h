#pragma once

#include "CoreMinimal.h"

#include "NetworkBuilder.generated.h"

class ADimension;
class UBlockLogic;
class UConductorBlockLogic;
class UConveyorBlockLogic;
struct FVector3i;

UCLASS()
class EVOSPACE_API UNetworkBuilder : public UObject {
  GENERATED_BODY()

  public:
  void Initialize(ADimension *inOwner);

  void OnBlockAdded(UBlockLogic *block);
  void OnBlockRemoved(const FVector3i &pos);
  void OnBlockChanged(UBlockLogic *block);

  void Flush();

  private:
  void MarkBlockDirty(UBlockLogic *block);
  void MarkPositionDirty(const FVector3i &pos);
  void RebuildConductors();
  void RebuildConveyors();

  UPROPERTY()
  TWeakObjectPtr<ADimension> ownerDimension;

  UPROPERTY()
  TSet<TObjectPtr<UConductorBlockLogic>> pendingConductors;

  UPROPERTY()
  TSet<TObjectPtr<UConveyorBlockLogic>> pendingConveyors;

  bool bFlushing = false;
};
