#include "Evospace/Shared/Public/LogicProgram.h"

#include "Evospace/Shared/Public/Condition.h"
#include "Evospace/Shared/Public/BlockLogic.h"
#include "Evospace/JsonHelper.h"

void ULogicProgram::Execute(UBlockLogic *Owner, ULogicContext *Ctx) {
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
void ULogicNode_Constant::Execute(UBlockLogic *Owner, ULogicContext *Ctx) {
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
      if (auto *item = FindObject<UStaticItem>(ANY_PACKAGE, *kv.Key)) {
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

// Arithmetic (minimal: Add/Sub over Output map)
void ULogicNode_Arithmetic::Execute(UBlockLogic *Owner, ULogicContext *Ctx) {
  const bool bSub = (Operation == FName("Sub"));
  for (auto &Pair : Ctx->Output->Map) {
    int64 &v = Pair.Value;
    if (bSub)
      v = -v; // simplistic placeholder
    // Add: no-op; combine will happen when maps merge in following nodes
  }
}

// Decider placeholder
void ULogicNode_Decider::Execute(UBlockLogic *Owner, ULogicContext *Ctx) {
  // For MVP rely on existing UCondition in ULogicCircuitBlockLogic Tick
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

void ULogicNode_Latch::Execute(UBlockLogic *Owner, ULogicContext *Ctx) {
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
void ULogicNode_ReadNetwork::Execute(UBlockLogic *Owner, ULogicContext *Ctx) {
  // Already aggregated into Ctx->Input by network layer
}

void ULogicNode_WriteNetwork::Execute(UBlockLogic *Owner, ULogicContext *Ctx) {
  // Network layer reads Ctx->Output and distributes
}

// Read/Control Machine placeholders
void ULogicNode_ReadMachine::Execute(UBlockLogic *Owner, ULogicContext *Ctx) {
  if (Owner)
    Owner->PopulateLogicOutput(Ctx);
}

void ULogicNode_ControlMachine::Execute(UBlockLogic *Owner, ULogicContext *Ctx) {
  if (Owner)
    Owner->ApplyLogicInput(Ctx);
}
