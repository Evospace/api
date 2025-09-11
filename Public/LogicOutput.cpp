#include "Public/LogicOutput.h"
#include "Public/LogicContext.h"
#include "Public/ItemMap.h"
#include "Public/StaticItem.h"
#include "Qr/JsonHelperCommon.h"

namespace {
    int64 ComputeExprOp(int64 a, int64 b, ELogicExprOp ExprOp) {
        switch (ExprOp) {
            case ELogicExprOp::Add: return a + b;
            case ELogicExprOp::Subtract: return a - b;
            case ELogicExprOp::Multiply: return a * b;
            case ELogicExprOp::Divide: return b == 0 ? 0 : a / b;
            case ELogicExprOp::Modulo: return b == 0 ? 0 : a % b;
            case ELogicExprOp::Power: {
            // Simple integer power implementation
            if (b == 0) return 1;
            if (b == 1) return a;
            if (b < 0) return 0; // Negative powers not supported for integers
        
            int64 result = 1;
            for (int64 i = 0; i < b; ++i) {
                result *= a;
            }
            return result;
            }
            default: return a;
        }
    }
}

void ULogicOutput::Compute(class ULogicContext *Ctx) const {
  if (!Ctx || !Ctx->Output)
    return;

  // Handle special pseudo-signals "Anything" and "Everything"
  const bool bAnything = (SignalA && SignalA->GetFName() == FName("Anything"));
  const bool bEverything = (SignalA && SignalA->GetFName() == FName("Everything"));

  if (bEverything) {
    if (!Ctx->Input) return;

    if (Mode == ELogicOutputMode::CopyA) {
      // Copy all input signals to all output signals
      for (const auto &kv : Ctx->Input->Map) {
        Ctx->Output->Add(kv.Key, kv.Value);
      }
    } else if (Mode == ELogicOutputMode::Constant) {
      // Write constant to all input signals
      for (const auto &kv : Ctx->Input->Map) {
        Ctx->Output->Add(kv.Key, ConstA);
      }
    } else if (Mode == ELogicOutputMode::Expression) {
      // Compute expression for each input signal
      for (const auto &kv : Ctx->Input->Map) {
        const int64 a = kv.Value;
        const int64 b = UseConstB ? ConstB : (Ctx->Input ? Ctx->Input->Get(SignalB) : 0);
        const int64 result = ComputeExprOp(a, b, ExprOp);
        Ctx->Output->Add(kv.Key, result);
      }
    }
    return;
  }

  if (bAnything) {
    if (!Ctx->Input) return;

    // Choose first available input signal
    const UStaticItem *chosenSignal = nullptr;
    int64 chosenValue = 0;
    for (const auto &kv : Ctx->Input->Map) {
      chosenSignal = kv.Key;
      chosenValue = kv.Value;
      break;
    }
    if (!chosenSignal) return;

    if (Mode == ELogicOutputMode::CopyA) {
      Ctx->Output->Add(chosenSignal, chosenValue);
    } else if (Mode == ELogicOutputMode::Constant) {
      Ctx->Output->Add(chosenSignal, ConstA);
    } else if (Mode == ELogicOutputMode::Expression) {
      const int64 a = chosenValue;
      const int64 b = UseConstB ? ConstB : (Ctx->Input ? Ctx->Input->Get(SignalB) : 0);
      const int64 result = ComputeExprOp(a, b, ExprOp);
      Ctx->Output->Add(chosenSignal, result);
    }
    return;
  }

  // Handle regular signals
  if (!SignalA) return;

  if (Mode == ELogicOutputMode::CopyA) {
    if (Ctx->Input) {
      Ctx->Output->Add(SignalA, Ctx->Input->Get(SignalA));
    }
  } else if (Mode == ELogicOutputMode::Constant) {
    Ctx->Output->Add(SignalA, ConstA);
  } else if (Mode == ELogicOutputMode::Expression) {
    const int64 a = Ctx->Input ? Ctx->Input->Get(SignalA) : 0;
    const int64 b = UseConstB ? ConstB : (Ctx->Input ? Ctx->Input->Get(SignalB) : 0);
    const int64 result = ComputeExprOp(a, b, ExprOp);
    Ctx->Output->Add(SignalA, result);
  }
}

bool ULogicOutput::DeserializeJson(TSharedPtr<FJsonObject> json) {
  json_helper::TryFind(json, TEXT("SigA"), SignalA);

  int32 mode;
  if (json_helper::TryGet(json, TEXT("Mode"), mode)) {
    Mode = static_cast<ELogicOutputMode>(mode);
  }

  json_helper::TryGet(json, TEXT("ConstA"), ConstA);

  // Expression mode properties
  int32 exprOp;
  if (json_helper::TryGet(json, TEXT("ExprOp"), exprOp)) {
    ExprOp = static_cast<ELogicExprOp>(exprOp);
  }

  json_helper::TryGet(json, TEXT("UseConstB"), UseConstB);
  json_helper::TryGet(json, TEXT("ConstB"), ConstB);
  json_helper::TryFind(json, TEXT("SigB"), SignalB);

  return true;
}

bool ULogicOutput::SerializeJson(TSharedPtr<FJsonObject> json) {
  json_helper::TrySet(json, TEXT("SigA"), SignalA);
  json_helper::TrySet(json, TEXT("Mode"), static_cast<int32>(Mode));
  json_helper::TrySet(json, TEXT("ConstA"), ConstA);

  // Expression mode properties
  json_helper::TrySet(json, TEXT("ExprOp"), static_cast<int32>(ExprOp));
  json_helper::TrySet(json, TEXT("UseConstB"), UseConstB);
  json_helper::TrySet(json, TEXT("ConstB"), ConstB);
  json_helper::TrySet(json, TEXT("SigB"), SignalB);

  return true;
}