#pragma once
#include "CoreMinimal.h"
#include "SwitchInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class USwitchInterface : public UInterface {
  GENERATED_BODY()
};

class ISwitchInterface {
  GENERATED_BODY()

  public:
  UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
  void SetSwitch(bool val);

  UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
  bool GetSwitch() const;

  virtual void SetSwitch_Implementation(bool val) = 0;
  virtual bool GetSwitch_Implementation() const = 0;
};