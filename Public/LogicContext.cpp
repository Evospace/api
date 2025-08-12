#include "Evospace/Shared/Public/LogicContext.h"

#include "Evospace/Shared/Public/ItemMap.h"

ULogicContext::ULogicContext() {
  Input = CreateDefaultSubobject<UItemMap>(TEXT("Input"));
  Output = CreateDefaultSubobject<UItemMap>(TEXT("Output"));
}
