#pragma once

#include "Blueprint/UserWidget.h"
#include "Evospace/Shared/Public/LogicContext.h"
#include "Evospace/Shared/Public/ContextProvider.h"
#include "LogicWidget.generated.h"

UCLASS(BlueprintType)
class ULogicNodeWidget : public UUserWidget {
  GENERATED_BODY()
  public:
  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
  class ULogicNode *LogicProgram = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
  class ULogicProgram *Parent = nullptr;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
  TScriptInterface<ULogicContextProvider> Context;
};

UCLASS(BlueprintType)
class ULogicProgramWidget : public UUserWidget {
  GENERATED_BODY()
  public:
  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
  class ULogicProgram *LogicProgram = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
  class ULogicProgram *Parent = nullptr;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
  TScriptInterface<ULogicContextProvider> Context;
};
