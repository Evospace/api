// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Condition.h"

#include <Dom/JsonObject.h>
#include <Templates/SharedPointer.h>

#include "ContextProvider.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class ULogicContextProvider : public UInterface {
  GENERATED_BODY()
};

class ILogicContextProvider {
  GENERATED_BODY()

  public:
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  ULogicContext *GetContext();

  virtual ULogicContext *GetContext_Implementation() = 0;
};
