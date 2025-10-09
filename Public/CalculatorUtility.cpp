#include "CalculatorUtility.h"
#include <stack>

namespace {
int32 GetPrecedence(TCHAR op) {
  switch (op) {
  case '+':
  case '-':
    return 1;
  case '*':
  case '/':
    return 2;
  case '^':
    return 3;
  default:
    return 0;
  }
}

bool ApplyOperator(float a, float b, TCHAR op, float &Result) {
  switch (op) {
  case '+':
    Result = a + b;
    return true;
  case '-':
    Result = a - b;
    return true;
  case '*':
    Result = a * b;
    return true;
  case '/':
    if (b == 0.0f)
      return false;
    Result = a / b;
    return true;
  case '^':
    Result = FMath::Pow(a, b);
    return true;
  default:
    return false;
  }
}

bool ProcessTop(std::stack<float> &Values, std::stack<TCHAR> &Ops, FString &ErrorMessage) {
  if (Values.size() < 2 || Ops.empty()) {
    ErrorMessage = TEXT("Invalid expression");
    return false;
  }

  float b = Values.top();
  Values.pop();
  float a = Values.top();
  Values.pop();
  TCHAR op = Ops.top();
  Ops.pop();

  float result = 0.0f;
  if (!ApplyOperator(a, b, op, result)) {
    ErrorMessage = (op == '/') ? TEXT("Division by zero") : TEXT("Unknown operator");
    return false;
  }

  Values.push(result);
  return true;
}
} // namespace

FCalculatorUtilityResult UCalculatorUtility::EvaluateExpression(const FString &Expression) {
  FString error;

  std::stack<float> Values;
  std::stack<TCHAR> Ops;

  FString CleanExpr = Expression.Replace(TEXT(" "), TEXT(""));
  const TCHAR *expr = *CleanExpr;

  int32 i = 0;
  while (expr[i] != '\0') {
    TCHAR ch = expr[i];

    if (FChar::IsDigit(ch) || ch == '.') {
      FString NumberStr;
      while (FChar::IsDigit(expr[i]) || expr[i] == '.') {
        NumberStr.AppendChar(expr[i]);
        ++i;
      }
      Values.push(FCString::Atof(*NumberStr));
      continue;
    }

    if (ch == '(') {
      Ops.push(ch);
    } else if (ch == ')') {
      while (!Ops.empty() && Ops.top() != '(') {
        if (!ProcessTop(Values, Ops, error))
          return { error, false, 0.0f };
      }
      if (!Ops.empty() && Ops.top() == '(')
        Ops.pop();
      else {
        return { "Mismatched parentheses", false, 0.0f };
      }
    } else if (FString("+-*/^").Contains(FString::Chr(ch))) {
      while (!Ops.empty() && GetPrecedence(Ops.top()) >= GetPrecedence(ch)) {
        if (!ProcessTop(Values, Ops, error))
          return { error, false, 0.0f };
      }
      Ops.push(ch);
    } else {
      return { FString("Invalid character: ") + ch, false, 0.0f };
    }

    ++i;
  }

  while (!Ops.empty()) {
    if (!ProcessTop(Values, Ops, error))
      return { error, false, 0.0f };
  }

  if (Values.size() != 1) {
    return { "Invalid expression", false, 0.0f };
  }

  return { "", true, Values.top() };
}