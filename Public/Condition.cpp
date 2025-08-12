#include "Condition.h"
#include "Evospace/Shared/Public/LogicContext.h"
#include "Evospace/Shared/Public/ItemMap.h"
#include "Public/StaticItem.h"
#include "Evospace/JsonHelper.h"
#include "Evospace/PerformanceStat.h"

//DECLARE_CYCLE_STAT_EXTERN(TEXT("Tick Condition"), STAT_TickCondition, STATGROUP_BLOCKLOGIC);

int64 UCondition::Evaluate(const ULogicContext *ctx) const {
  //SCOPE_CYCLE_COUNTER(STAT_TickCondition);
  switch (Mode) {
  case EConditionMode::Expr: {
    int64 actual = ctx->Input->Get(VarA);
    int64 value = ConstValue;
    if (ConstB == false) {
      value = ctx->Input->Get(VarB);
    }
    switch (Operator) {
    case ECompareOp::Less:
      return actual < value;
    case ECompareOp::Greater:
      return actual > value;
    case ECompareOp::Equal:
      return actual == value;
    case ECompareOp::NotEqual:
      return actual != value;
    case ECompareOp::LessEqual:
      return actual <= value;
    case ECompareOp::GreaterEqual:
      return actual >= value;
    default:
      checkNoEntry();
    }
    return 0;
  }
  case EConditionMode::Or: {
    if (Operands.IsEmpty())
      return 1;
    for (const auto *cond : Operands)
      if (cond->Evaluate(ctx)) return 1;
    return 0;
  }
  case EConditionMode::And: {
    for (const auto *cond : Operands)
      if (!cond->Evaluate(ctx)) return 0;
    return 1;
  }
  case EConditionMode::Not:
    return (Operands.Num() == 1 && Operands[0]) ? !Operands[0]->Evaluate(ctx) : 1;
  case EConditionMode::Always:
  default:
    return 1;
  }
}

float UCondition::EvaluateGui(const ULogicContext *ctx) const {
  //SCOPE_CYCLE_COUNTER(STAT_TickCondition);
  switch (Mode) {
  case EConditionMode::Expr: {
    float actual = ctx->Input->Get(VarA);
    float value = ConstValue;
    if (!ConstB) {
      value = ctx->Input->Get(VarB);
    }

    float delta = actual - value;

    switch (Operator) {
    case ECompareOp::Less:
      if (actual < value) return 1.f;
      return value == 0.f ? 0.f : FMath::Clamp(1.f - (actual / value), 0.f, 1.f);

    case ECompareOp::Greater:
      if (actual > value) return 1.f;
      return value == 0.f ? 0.f : FMath::Clamp(actual / value, 0.f, 1.f);

    case ECompareOp::Equal: {
      float eps = FMath::Max(1.f, FMath::Abs(value) * 0.01f);
      if (FMath::Abs(delta) < eps * 0.25f) return 1.f;
      return 1.f - FMath::Clamp(FMath::Abs(delta) / eps, 0.f, 1.f);
    }

    case ECompareOp::NotEqual: {
      float eps = FMath::Max(1.f, FMath::Abs(value) * 0.01f);
      if (FMath::Abs(delta) > eps * 2.f) return 1.f;
      return FMath::Clamp(FMath::Abs(delta) / eps, 0.f, 1.f);
    }

    case ECompareOp::LessEqual:
      if (actual <= value) return 1.f;
      return value == 0.f ? 0.f : FMath::Clamp(1.f - (actual / value), 0.f, 1.f);

    case ECompareOp::GreaterEqual:
      if (actual >= value) return 1.f;
      return value == 0.f ? 0.f : FMath::Clamp(actual / value, 0.f, 1.f);

    default:
      checkNoEntry();
    }
    return 0.f;
  }
  case EConditionMode::Or: {
    if (Operands.IsEmpty())
      return 1.0;
    for (const auto *cond : Operands)
      if (cond->EvaluateGui(ctx) > 0.999) return 1.0;
    return 0.0;
  }
  case EConditionMode::And: {
    for (const auto *cond : Operands)
      if (cond->EvaluateGui(ctx) < 0.999) return 0.0;
    return 1.0;
  }
  case EConditionMode::Not:
    return (Operands.Num() == 1 && Operands[0]) ? Operands[0]->EvaluateGui(ctx) <= 0.999 : 1.0;
  case EConditionMode::Always:
  default:
    return 1.0;
  }
}

void UCondition::RemoveOperand(UCondition *op) {
  Operands.Remove(op);
}

TSubclassOf<UConditionWidget> UCondition::GetWidgetClass() const {
  return LoadObject<UClass>(nullptr, TEXT("/Game/Gui/Logic/ConditionBlock.ConditionBlock_C"));
}

FString UCondition::GetLabel() const {
  switch (Mode) {
  case EConditionMode::Expr:
    return "Expr";
  case EConditionMode::Or:
    return "Or";
  case EConditionMode::And:
    return "And";
  case EConditionMode::Xor:
    return "Xor";
  case EConditionMode::Not:
    return "Not";
  case EConditionMode::Always:
    return "Always";
  default:
    return "Unknown";
  }
}

void UCondition::Reset() {
  VarA = nullptr;
  VarB = nullptr;
  ConstB = true;
  Operator = ECompareOp::Less;
  ConstValue = 0;
  Mode = EConditionMode::Expr;
  Operands.Reset();
  OutputSignal = nullptr;
  OutputValueTrue = 1;
  OutputValueFalse = 0;
}

bool UCondition::DeserializeJson(TSharedPtr<FJsonObject> json) {
  int32 value = 0;

  Reset();

  json_helper::TryFind(json, "A", VarA);
  json_helper::TryFind(json, "B", VarB);
  json_helper::TryGet(json, "Cb", ConstB);
  if (json_helper::TryGet(json, "Op", value)) {
    Operator = static_cast<ECompareOp>(value);
  }
  json_helper::TryGet(json, "Cv", ConstValue);
  if (json_helper::TryGet(json, "M", value)) {
    Mode = static_cast<EConditionMode>(value);
  }
  json_helper::TryDeserialize(json, "Arg", Operands);

  // Output wiring
  json_helper::TryFind(json, "OutSig", OutputSignal);
  json_helper::TryGet(json, "OutT", OutputValueTrue);
  json_helper::TryGet(json, "OutF", OutputValueFalse);
  return true;
}

bool UCondition::SerializeJson(TSharedPtr<FJsonObject> json) {
  if (VarA != nullptr)
    json_helper::TrySet(json, "A", VarA);
  if (VarB != nullptr)
    json_helper::TrySet(json, "B", VarB);
  if (ConstB != true)
    json_helper::TrySet(json, "Cb", ConstB);
  if (Operator != ECompareOp::Less) {
    int32 value = static_cast<int32>(Operator);
    json_helper::TrySet(json, "Op", value);
  }
  if (ConstValue != 0) {
    json_helper::TrySet(json, "Cv", ConstValue);
  }
  if (Mode != EConditionMode::Expr) {
    int32 value = static_cast<int32>(Mode);
    json_helper::TrySet(json, "M", value);
  }
  json_helper::TrySerialize(json, "Arg", Operands);

  if (OutputSignal)
    json_helper::TrySet(json, "OutSig", OutputSignal);
  if (OutputValueTrue != 1)
    json_helper::TrySet(json, "OutT", OutputValueTrue);
  if (OutputValueFalse != 0)
    json_helper::TrySet(json, "OutF", OutputValueFalse);

  return true;
}
