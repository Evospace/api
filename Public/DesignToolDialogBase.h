// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <Blueprint/UserWidget.h>

#include "Evospace/Shared/Public/StaticCover.h"
#include "Evospace/Shared/Public/DesignToolSupportInterface.h"
#include "DesignToolDialogBase.generated.h"

class UStaticCoverSet;

/**
 * Base widget for Design Tool dialog. Holds available designs and calls back into the block via interface.
 */
UCLASS(BlueprintType)
class UDesignToolDialogBase : public UUserWidget {
  GENERATED_BODY()

  public:
  UFUNCTION(BlueprintCallable)
  void SetContext(TScriptInterface<IDesignToolSupportInterface> supportInterface, UStaticCoverSet *available) {
    DesignToolSupportInterface = supportInterface;
    AvailableCovers = available;
    OnContextSet();
  }

  UFUNCTION(BlueprintCallable, BlueprintPure)
  UStaticCoverSet *GetAvailableCovers() const { return AvailableCovers; }

  UFUNCTION(BlueprintCallable)
  void SelectCover(UStaticCover *cover);

  UFUNCTION(BlueprintCallable)
  void SetColor(const FColor &color, int32 colorIndex);

  UFUNCTION(BlueprintImplementableEvent)
  void OnContextSet();

  protected:
  UPROPERTY(BlueprintReadOnly)
  TScriptInterface<IDesignToolSupportInterface> DesignToolSupportInterface = nullptr;

  UPROPERTY(BlueprintReadOnly)
  UStaticCoverSet *AvailableCovers = nullptr;
};


