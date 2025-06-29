#include "GuiTextHelper.h"


FText UGuiTextHelper::FloatToGuiCount(float count, int32 afterdigit, bool no_zero, int32 min_afterdigit) {
    FNumberFormattingOptions NumberFormat;
    NumberFormat.MinimumIntegralDigits = 1;
    NumberFormat.MaximumIntegralDigits = 10000;
    NumberFormat.MinimumFractionalDigits = min_afterdigit;
    NumberFormat.MaximumFractionalDigits = afterdigit;

    if (FMath::IsNearlyZero(count)) {
      if (no_zero)
        return FText::GetEmpty();
      else
        return FText::FromString(TEXT("0 "));
    }

    if (count < 0) {
      if (count < -1.e21f) {
        return FText::FromString(
          FText::AsNumber(count / 1.e18f, &NumberFormat).ToString() +
          TEXT(" P"));
      }

      if (count < -1.e18f) {
        return FText::FromString(
          FText::AsNumber(count / 1.e15f, &NumberFormat).ToString() +
          TEXT(" T"));
      }

      if (count < -1.e15f) {
        return FText::FromString(
          FText::AsNumber(count / 1.e12f, &NumberFormat).ToString() +
          TEXT(" G"));
      }

      if (count < -1.e12f) {
        return FText::FromString(
          FText::AsNumber(count / 1.e9f, &NumberFormat).ToString() +
          TEXT(" G"));
      }

      if (count < -1.e9f) {
        return FText::FromString(
          FText::AsNumber(count / 1.e6f, &NumberFormat).ToString() +
          TEXT(" M"));
      }

      if (count < -1.e6f) {
        return FText::FromString(
          FText::AsNumber(count / 1.e3f, &NumberFormat).ToString() +
          TEXT(" k"));
      }

      return FText::AsNumber(count, &NumberFormat);
    }

    if (count < 1.f / 100.f) {
      return FText::FromString(FText::AsNumber(count * 1000.f, &NumberFormat).ToString() + TEXT(" m"));
    }

    if (count < 1.e3f) {
      return FText::FromString(FText::AsNumber(count, &NumberFormat).ToString() + " ");
    }

    if (count < 1.e6f) {
      return FText::FromString(
        FText::AsNumber(count / 1.e3f, &NumberFormat).ToString() + TEXT(" k"));
    }

    if (count < 1.e9f) {
      return FText::FromString(
        FText::AsNumber(count / 1.e6f, &NumberFormat).ToString() + TEXT(" M"));
    }

    if (count < 1.e12f) {
      return FText::FromString(
        FText::AsNumber(count / 1.e9f, &NumberFormat).ToString() + TEXT(" G"));
    }

    if (count < 1.e15f) {
      return FText::FromString(
        FText::AsNumber(count / 1.e12f, &NumberFormat).ToString() + TEXT(" G"));
    }

    if (count < 1.e18f) {
      return FText::FromString(
        FText::AsNumber(count / 1.e15f, &NumberFormat).ToString() + TEXT(" T"));
    }

    if (count < 1.e21f) {
      return FText::FromString(
        FText::AsNumber(count / 1.e18f, &NumberFormat).ToString() + TEXT(" P"));
    }

    return FText::FromString(FText::AsNumber(count / 1.e21f, &NumberFormat).ToString() + TEXT(" E"));
  }