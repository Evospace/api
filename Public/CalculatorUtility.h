#pragma once

#include "CalculatorUtility.generated.h"

struct FCalculatorUtilityResult {
  FString error;
  bool bSuccess;
  float result;
};

UCLASS()
class UCalculatorUtility : public UObject {
  GENERATED_BODY()
  public:
  static FCalculatorUtilityResult EvaluateExpression(const FString &Expression);
};