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

  UPROPERTY()
  UResourceAccessor *acc;

  public:
  virtual bool IsUniversalCrafter() const override;

  virtual void SelectRecipeIndex(APlayerController *pc, int32 i) override;

  virtual void SelectRecipe(APlayerController *pc, const URecipe *recipe) override;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  virtual TSubclassOf<UBlockWidget> GetWidgetClass() const override;

  int32 AnimationTicks = 0;
};