#pragma once

#include "CoreMinimal.h"
#include "EvoRingBuffer.h"
#include "Blueprint/UserWidget.h"
#include "FpsGraphWidget.generated.h"

class UFPSCounterSubsystem;

UCLASS(BlueprintType, Blueprintable)
class UPerformanceGraphWidget : public UUserWidget {
  GENERATED_BODY()

  protected:
  virtual int32 NativePaint(const FPaintArgs &Args, const FGeometry &AllottedGeometry, const FSlateRect &MyCullingRect,
                            FSlateWindowElementList &OutDrawElements, int32 LayerId,
                            const FWidgetStyle &InWidgetStyle, bool bParentEnabled) const override;

  virtual void NativeTick(const FGeometry &MyGeometry, float InDeltaTime) override;

  private:
  float deltaAccum = 0;
  int last = 0;
  TWeakObjectPtr<UFPSCounterSubsystem> FPSSubsystem;
  EvoRingBuffer<float> FpsBuffer = EvoRingBuffer<float>(64, 0);
};