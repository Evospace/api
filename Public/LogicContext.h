#pragma once

#include "CoreMinimal.h"
#include "LogicContext.generated.h"

class UItemMap;
class UStaticItem;

UCLASS(BlueprintType)
class ULogicContext : public UObject {
  GENERATED_BODY()

  public:
  ULogicContext();

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UItemMap *Input;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UItemMap *Output;
};
