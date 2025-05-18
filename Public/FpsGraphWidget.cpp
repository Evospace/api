#include "FpsGraphWidget.h"

#include "Engine/World.h"
#include "SlateOptMacros.h"
#include "Evospace/FpsSubsystem.h"
#include "Widgets/SCanvas.h"
#include "Rendering/DrawElements.h"

void UPerformanceGraphWidget::NativeTick(const FGeometry &MyGeometry, float InDeltaTime) {
  Super::NativeTick(MyGeometry, InDeltaTime);

  deltaAccum += InDeltaTime;

  if (deltaAccum >= 0.25) {
    deltaAccum -= 0.25;
    if (!FPSSubsystem.IsValid()) {
      FPSSubsystem = GetWorld()->GetSubsystem<UFPSCounterSubsystem>();
    }

    FpsBuffer.Push(FPSSubsystem->GetRawFPS());
    ++last;
    if (last >= FpsBuffer.Capacity())
      last = 0;

    Invalidate(EInvalidateWidget::LayoutAndVolatility);
  }
}

int32 UPerformanceGraphWidget::NativePaint(const FPaintArgs &Args, const FGeometry &AllottedGeometry,
                                           const FSlateRect &MyCullingRect, FSlateWindowElementList &OutDrawElements,
                                           int32 LayerId, const FWidgetStyle &InWidgetStyle, bool bParentEnabled) const {
  Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

  if (!FPSSubsystem.IsValid())
    return LayerId;

  const auto &Buffer = FpsBuffer.GetData();
  const int32 NumPoints = Buffer.Num();
  if (NumPoints < 2) return LayerId;

  const FVector2D Size = AllottedGeometry.GetLocalSize();
  float MaxFPS = 120.0f;
  float MinFPS = 0.0f;

  TArray<FVector2D> Points;
  for (int32 i = last; i < last + NumPoints; ++i) {
    int index = i % NumPoints;
    float Normalized = FMath::Clamp((Buffer[index] - MinFPS) / (MaxFPS - MinFPS), 0.0f, 1.0f);
    float X = Size.X * (i - last) / float(NumPoints - 1);
    float Y = Size.Y * (1.0f - Normalized);
    Points.Add(FVector2D(X, Y));
  }

  TArray<FSlateVertex> Vertices;
  TArray<SlateIndex> Indices;
  for (int32 i = 0; i < Points.Num() - 1; ++i) {
    FVector2D P1 = Points[i];
    FVector2D P2 = Points[i + 1];
    FSlateDrawElement::MakeLines(
      OutDrawElements,
      LayerId,
      AllottedGeometry.ToPaintGeometry(),
      { P1, P2 },
      ESlateDrawEffect::None,
      FLinearColor::Green,
      true,
      2.0f);
  }

  return LayerId + 1;
}