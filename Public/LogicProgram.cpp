#include "Evospace/Shared/Public/LogicProgram.h"
#include "Evospace/Shared/Public/LogicInterface.h"
#include "Evospace/Shared/Public/Condition.h"
#include "Evospace/Shared/Public/LogicContext.h"
#include "Evospace/Shared/Public/ItemMap.h"
#include "Evospace/Shared/Public/BlockLogic.h"
#include "Evospace/Shared/Qr/QrFind.h"
#include "Evospace/JsonHelper.h"

void ULogicProgram::Execute(TScriptInterface<ILogicInterface> Owner, ULogicContext *Ctx) {
  if (!Ctx)
    return;
  for (auto *Node : Nodes) {
    if (Node)
      Node->Execute(Owner, Ctx);
  }
}

bool ULogicProgram::DeserializeJson(TSharedPtr<FJsonObject> json) {
  return json_helper::TryDeserialize(json, TEXT("Nodes"), Nodes);
}

bool ULogicProgram::SerializeJson(TSharedPtr<FJsonObject> json) {
  json_helper::TrySerialize(json, TEXT("Nodes"), Nodes);
  return true;
}

// Constant
void ULogicNode_Constant::Execute(TScriptInterface<ILogicInterface> Owner, ULogicContext *Ctx) {
  for (const auto &kv : Values) {
    Ctx->Output->Add(kv.Key, kv.Value);
  }
}

bool ULogicNode_Constant::DeserializeJson(TSharedPtr<FJsonObject> json) {
  // Store as map of item path -> value
  TMap<FString, int64> Raw;
  if (json_helper::TryGet(json, TEXT("Values"), Raw)) {
    Values.Empty();
    for (const auto &kv : Raw) {
      if (auto *item = QrFind<UStaticItem>(*kv.Key)) {
        Values.Add(item, kv.Value);
      }
    }
  }
  return true;
}

bool ULogicNode_Constant::SerializeJson(TSharedPtr<FJsonObject> json) {
  TMap<FString, int64> Raw;
  for (const auto &kv : Values) {
    if (kv.Key)
      Raw.Add(kv.Key->GetPathName(), kv.Value);
  }
  json_helper::TrySet(json, TEXT("Values"), Raw);
  return true;
}

// Arithmetic
void ULogicNode_Arithmetic::Execute(TScriptInterface<ILogicInterface> Owner, ULogicContext *Ctx) {
  if (!Ctx || !Ctx->Output)
    return;

  if (!SourceSignal)
    return;

  auto apply = [&](int64 a, int64 b) -> int64 {
    if (Operation == TEXT("Add")) return a + b;
    if (Operation == TEXT("Sub")) return a - b;
    if (Operation == TEXT("Mul")) return a * b;
    if (Operation == TEXT("Div")) return b == 0 ? 0 : a / b;
    if (Operation == TEXT("Mod")) return b == 0 ? 0 : a % b;
    return a;
  };

  const int64 a = Ctx->Input ? Ctx->Input->Get(SourceSignal) : 0;
  const int64 b = UseConstB ? ConstB : (Ctx->Input ? Ctx->Input->Get(SignalB) : 0);
  const int64 res = apply(a, b);

  UStaticItem *dst = OutputSignal ? OutputSignal : SourceSignal;
  Ctx->Output->Set(dst, res);
}

// Decider
void ULogicNode_Decider::Execute(TScriptInterface<ILogicInterface> Owner, ULogicContext *Ctx) {
  if (!Ctx || !Ctx->Output || !Condition || !OutputSignal)
    return;

  const bool bPassed = Condition->Evaluate(Ctx) != 0;

  auto writeCopyA = [&]() {
    const int64 val = (Condition->VarA ? Ctx->Input->Get(Condition->VarA) : 0);
    Ctx->Output->Set(OutputSignal, val);
  };

  if (bPassed) {
    switch (OutputMode) {
    case EDeciderOutputMode::Constant:
      Ctx->Output->Set(OutputSignal, OutputValueTrue);
      break;
    case EDeciderOutputMode::CopyA:
      writeCopyA();
      break;
    default:
      break;
    }
  } else {
    switch (FalseBehavior) {
    case EDeciderFalseBehavior::DoNothing:
      break;
    case EDeciderFalseBehavior::WriteZero:
      Ctx->Output->Set(OutputSignal, OutputValueFalse);
      break;
    case EDeciderFalseBehavior::CopyA:
      writeCopyA();
      break;
    default:
      break;
    }
  }
}

bool ULogicNode_Decider::DeserializeJson(TSharedPtr<FJsonObject> json) {
  if (!Condition) {
    Condition = NewObject<UCondition>(this, UCondition::StaticClass());
  }
  json_helper::TryDeserialize(json, TEXT("Cond"), Condition);
  json_helper::TryFind(json, TEXT("OutSig"), OutputSignal);

  int32 modeInt = static_cast<int32>(OutputMode);
  if (json_helper::TryGet(json, TEXT("Mode"), modeInt))
    OutputMode = static_cast<EDeciderOutputMode>(modeInt);

  json_helper::TryGet(json, TEXT("OutT"), OutputValueTrue);

  int32 fbInt = static_cast<int32>(FalseBehavior);
  if (json_helper::TryGet(json, TEXT("False"), fbInt))
    FalseBehavior = static_cast<EDeciderFalseBehavior>(fbInt);

  json_helper::TryGet(json, TEXT("OutF"), OutputValueFalse);
  return true;
}

bool ULogicNode_Decider::SerializeJson(TSharedPtr<FJsonObject> json) {
  json_helper::TrySerialize(json, TEXT("Cond"), Condition);
  if (OutputSignal)
    json_helper::TrySet(json, TEXT("OutSig"), OutputSignal);

  if (OutputMode != EDeciderOutputMode::Constant) {
    int32 modeInt = static_cast<int32>(OutputMode);
    json_helper::TrySet(json, TEXT("Mode"), modeInt);
  }

  if (OutputValueTrue != 1)
    json_helper::TrySet(json, TEXT("OutT"), OutputValueTrue);

  if (FalseBehavior != EDeciderFalseBehavior::DoNothing) {
    int32 fbInt = static_cast<int32>(FalseBehavior);
    json_helper::TrySet(json, TEXT("False"), fbInt);
  }

  if (OutputValueFalse != 0)
    json_helper::TrySet(json, TEXT("OutF"), OutputValueFalse);
  return true;
}

bool ULogicNode_Arithmetic::DeserializeJson(TSharedPtr<FJsonObject> json) {
  json_helper::TryGet(json, TEXT("Op"), Operation);
  json_helper::TryFind(json, TEXT("A"), SourceSignal);
  json_helper::TryGet(json, TEXT("UseConstB"), UseConstB);
  json_helper::TryGet(json, TEXT("ConstB"), ConstB);
  json_helper::TryFind(json, TEXT("SigB"), SignalB);
  json_helper::TryFind(json, TEXT("OutSig"), OutputSignal);
  return true;
}

bool ULogicNode_Arithmetic::SerializeJson(TSharedPtr<FJsonObject> json) {
  if (!Operation.IsNone())
    json_helper::TrySet(json, TEXT("Op"), Operation);
  if (SourceSignal)
    json_helper::TrySet(json, TEXT("A"), SourceSignal);
  if (UseConstB == false)
    json_helper::TrySet(json, TEXT("UseConstB"), UseConstB);
  if (ConstB != 0)
    json_helper::TrySet(json, TEXT("ConstB"), ConstB);
  if (SignalB)
    json_helper::TrySet(json, TEXT("SigB"), SignalB);
  if (OutputSignal)
    json_helper::TrySet(json, TEXT("OutSig"), OutputSignal);
  return true;
}

// Latch
bool ULogicNode_Latch::DeserializeJson(TSharedPtr<FJsonObject> json) {
  json_helper::TryGet(json, TEXT("State"), State);
  return true;
}

bool ULogicNode_Latch::SerializeJson(TSharedPtr<FJsonObject> json) {
  json_helper::TrySet(json, TEXT("State"), State);
  return true;
}

void ULogicNode_Latch::Execute(TScriptInterface<ILogicInterface>Owner, ULogicContext *Ctx) {
  // Set/Reset by special signals (e.g., Signal_Set, Signal_Reset)
  const UStaticItem *SigSet = FindObject<UStaticItem>(ANY_PACKAGE, TEXT("Signal_Set"));
  const UStaticItem *SigReset = FindObject<UStaticItem>(ANY_PACKAGE, TEXT("Signal_Reset"));
  if (SigSet && Ctx->Input->Has(SigSet))
    State = true;
  if (SigReset && Ctx->Input->Has(SigReset))
    State = false;
  // Expose state as Signal_Latch
  const UStaticItem *SigLatch = FindObject<UStaticItem>(ANY_PACKAGE, TEXT("Signal_Latch"));
  if (SigLatch)
    Ctx->Output->Set(SigLatch, State ? 1 : 0);
}

// Read/Write Network placeholder
void ULogicNode_ReadNetwork::Execute(TScriptInterface<ILogicInterface> Owner, ULogicContext *Ctx) {
  // Already aggregated into Ctx->Input by network layer
}

void ULogicNode_WriteNetwork::Execute(TScriptInterface<ILogicInterface> Owner, ULogicContext *Ctx) {
  // Network layer reads Ctx->Output and distributes
}

// Read/Control Machine placeholders
void ULogicNode_ReadMachine::Execute(TScriptInterface<ILogicInterface> Owner, ULogicContext *Ctx) {
  if (Owner)
    Owner->PopulateLogicOutput(Ctx);
}

void ULogicNode_ControlMachine::Execute(TScriptInterface<ILogicInterface> Owner, ULogicContext *Ctx) {
  if (Owner)
    Owner->ApplyLogicInput(Ctx);
}

TSubclassOf<ULogicProgramWidget> ULogicProgram::GetWidgetClass() const {
  return LoadObject<UClass>(nullptr, TEXT("/Game/Gui/Logic/LogicProgramWidgetBP.LogicProgramWidgetBP_C"));
}

TSubclassOf<ULogicNodeWidget> ULogicNode::GetWidgetClass() const {
  return LoadObject<UClass>(nullptr, TEXT("/Game/Gui/Logic/LogicNodeWidgetBP.LogicNodeWidgetBP_C"));
}