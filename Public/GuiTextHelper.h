#pragma once

#include "GuiTextHelper.generated.h"

UCLASS()
class UGuiTextHelper : public UObject {
  GENERATED_BODY()
  public:
  UFUNCTION(BlueprintCallable, BlueprintPure)
  static FText FloatToGuiCount(float count, int32 afterdigit = 0, bool no_zero = true, int32 min_afterdigit = 0);
};