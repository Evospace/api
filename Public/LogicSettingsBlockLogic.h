#pragma once

#include "CoreMinimal.h"
#include "Public/BlockLogic.h"

#include "LogicSettingsBlockLogic.generated.h"

class ULogicSettings;

UCLASS(BlueprintType)
class ULogicSettingsBlockLogic : public UBlockLogic {
  GENERATED_BODY()

  public:
  virtual ULogicSettings *GetLogicSettings() override;
};