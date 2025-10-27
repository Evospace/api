// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <Blueprint/UserWidget.h>

#include "Evospace/Shared/Public/StaticCover.h"
#include "DesignToolDialogBase.generated.h"

class ADesignToolItemLogic;
class UStaticCoverSet;

/**
 * Base widget for Design Tool dialog. Holds available designs and calls back into the tool when user selects one.
 */
UCLASS(BlueprintType)
class UDesignToolDialogBase : public UUserWidget {
  GENERATED_BODY()

  public:
  UFUNCTION(BlueprintCallable)
  void SetContext(ADesignToolItemLogic *toolLogic, UStaticCoverSet *available) {
    ToolLogic = toolLogic;
    AvailableCovers = available;
    OnContextSet();
  }

  UFUNCTION(BlueprintCallable, BlueprintPure)
  UStaticCoverSet *GetAvailableCovers() const { return AvailableCovers; }

  UFUNCTION(BlueprintCallable)
  void SelectCover(UStaticCover *cover);

  UFUNCTION(BlueprintImplementableEvent)
  void OnContextSet();

  protected:
  UPROPERTY(BlueprintReadOnly)
  ADesignToolItemLogic *ToolLogic = nullptr;

  UPROPERTY(BlueprintReadOnly)
  UStaticCoverSet *AvailableCovers = nullptr;
};


