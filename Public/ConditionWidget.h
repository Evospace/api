// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Public/ContextProvider.h"
#include "ConditionWidget.generated.h"

class UCondition;

// Base class for condition editor widgets (ConditionBlock/ConditionBlockUi BPs).
UCLASS(BlueprintType)
class UConditionWidget : public UUserWidget {
  GENERATED_BODY()
  public:
  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
  UCondition *Condition = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
  UCondition *Parent = nullptr;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
  TScriptInterface<ULogicContextProvider> Context;
};
