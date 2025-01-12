#pragma once

#include "AbstractCrafter.h"
#include "DumpCrafter.generated.h"

UCLASS(BlueprintType)
class EVOSPACE_API UDumpCrafterBlockLogic : public UAbstractCrafter {
  GENERATED_BODY()

  protected:
  UDumpCrafterBlockLogic();

  virtual void Tick() override;

  virtual void BlockBeginPlay() override;

  virtual void SetActor(ABlockActor *actor) override;

  UPROPERTY(BlueprintReadOnly)
  FLinearColor Color;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  UInventory *InputInventory;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  UInventory *OutputInventory;

  public:
  virtual bool IsUniversalCrafter() const override;

  virtual void SelectRecipe(APlayerController *pc, int32 i) override;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  virtual TSubclassOf<UBlockWidget> GetWidgetClass() const override;

  int32 AnimationTicks = 0;
};