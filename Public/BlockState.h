#pragma once
#include "Evospace/Common.h"

#include "BlockState.generated.h"

UENUM(BlueprintType)
enum class EBlockState : uint8 {
  InputShortage,
  Working,
  NoRoomInOutput,
  ResourceSaturated,
  ResourceRequired,
  NotInitialized,
  SwitchOff,
  Idle
};

struct FBlockStateMeta {
  FName LabelKey;
  FLinearColor LampColor = (FLinearColor::Blue + FLinearColor::Red) / 2.0;
};

UCLASS()
class UCrafterStateLibrary : public UBlueprintFunctionLibrary {
  GENERATED_BODY()

  public:
  UFUNCTION(BlueprintPure, Category = "Crafter|Status")
  static FLoc GetLocKeys(EBlockState State);

  UFUNCTION(BlueprintPure, Category = "Crafter|Status")
  static FLinearColor GetLampColor(EBlockState State);
};