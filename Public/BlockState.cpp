#include "BlockState.h"

namespace {
static const FBlockStateMeta CrafterStateMap[] = {
  { "InputShortage", { 1.0f, 0.1f, 0.1f } },
  { "Working", { 0.1f, 1.0f, 0.1f } },
  { "NoRoomInOutput", { 1.0f, 1.0f, 0.1f } },
  { "ResourceSaturated", { 0.1f, 0.8f, 1.0f } },
  { "ResourceRequired", { 1.0f, 0.3f, 0.3f } },
  { "NotInitialized", { 0.6f, 0.1f, 1.0f } },
  { "SwitchOff", { 0.6f, 0.6f, 0.6f } },
  { "Idle", { 0.7f, 0.7f, 0.7f } },
};

static const FBlockStateMeta InvalidMeta{ "status_unknown", { 0.5f, 0.0f, 1.0f } };

FORCEINLINE const auto &GetMeta(EBlockState S) {
  const int32 Idx = static_cast<int32>(S);
  return (Idx >= 0 && Idx < UE_ARRAY_COUNT(CrafterStateMap)) ? CrafterStateMap[Idx] : InvalidMeta;
}
} // namespace

FLoc UCrafterStateLibrary::GetLocKeys(EBlockState State) {
  const auto &Meta = GetMeta(State);
  return { { Meta.LabelKey, "block_state" } };
}

FLinearColor UCrafterStateLibrary::GetLampColor(EBlockState State) {
  const auto &Meta = GetMeta(State);
  return Meta.LampColor;
}