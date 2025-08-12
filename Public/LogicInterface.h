// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Condition.h"
#include "Qr/Loc.h"

#include "LogicInterface.generated.h"

USTRUCT(Blueprintable)
struct FLogicExport {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UStaticItem *Item;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FLoc Loc;
};

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class ULogicInterface : public UInterface {
  GENERATED_BODY()
};

class ILogicInterface {
  GENERATED_BODY()

  public:
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  TArray<FLogicExport> GetExport();

  virtual TArray<FLogicExport> GetExport_Implementation() = 0;

  virtual void PopulateLogicOutput(class ULogicContext *ctx) const = 0;

  virtual void ApplyLogicInput(const class ULogicContext *ctx) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  TArray<UStaticItem *> GetFavoriteSignals() const;

  virtual TArray<UStaticItem *> GetFavoriteSignals_Implementation() const { return {}; }
};
