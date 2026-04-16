#pragma once

#include "CoreMinimal.h"
#include "Public/BlockLogic.h"

#include "LogicSettingsBlockLogic.generated.h"

class ULogicSettings;

UCLASS(BlueprintType)
class ULogicSettingsBlockLogic : public UBlockLogic {
  GENERATED_BODY()

  public:
  ULogicSettingsBlockLogic();

  virtual ULogicSettings *GetLogicSettings() override;

  virtual void LoadSettings(TSharedPtr<FJsonObject> json, AMainPlayerController *mpc = nullptr) override;
  virtual void SaveSettings(TSharedPtr<FJsonObject> json, AMainPlayerController *mpc = nullptr) const override;
};