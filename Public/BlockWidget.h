// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "../Common.h"
#include "Qr/SerializableJson.h"
#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"

#include "BlockWidget.generated.h"
class UBaseInventoryWidgetBase;
class UBlockLogic;
class UStaticBlock;

UCLASS(BlueprintType)
class UBlockWidget : public UUserWidget {
  GENERATED_BODY()

  public:
  TArray<UBaseInventoryWidgetBase *> GetInventoryWidgets();

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
  UBlockLogic *mBlockLogic = nullptr;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
  UStaticBlock *mStaticBlock = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
  EBlockWidgetType mBlockWidgetType = EBlockWidgetType::OnePanel;

  UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
  void UpdateRaw();

  UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
  bool mRaw = true;
};
