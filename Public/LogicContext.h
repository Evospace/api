#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LogicContext.generated.h"

class UItemMap;
class UStaticItem;

UCLASS(BlueprintType)
class EVOSPACE_API ULogicContext : public UObject {
  GENERATED_BODY()

  public:
  ULogicContext();

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UItemMap *Input;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UItemMap *Output;
};
