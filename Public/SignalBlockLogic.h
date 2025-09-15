#pragma once

#include "CoreMinimal.h"
#include "Public/BlockLogic.h"

#include "SignalBlockLogic.generated.h"

UCLASS(BlueprintType)
class USignalBlockLogic : public UBlockLogic {
  GENERATED_BODY()

  public:
  virtual ULogicSignal *GetSignal() override;
};