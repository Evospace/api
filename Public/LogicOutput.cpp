#include "Public/LogicOutput.h"
#include "Qr/JsonHelperCommon.h"

bool ULogicOutput::DeserializeJson(TSharedPtr<FJsonObject> json) {
  json_helper::TryFind(json, TEXT("Sig"), OutputSignal);
  int32 mode;
  if (json_helper::TryGet(json, TEXT("Mode"), mode)) {
    OutputMode = static_cast<ELogicOutputMode>(mode);
  }
  json_helper::TryGet(json, TEXT("Val"), OutputValueTrue);
  return true;
}

bool ULogicOutput::SerializeJson(TSharedPtr<FJsonObject> json) {
  json_helper::TrySet(json, TEXT("Sig"), OutputSignal);
  json_helper::TrySet(json, TEXT("Mode"), static_cast<int32>(OutputMode));
  json_helper::TrySet(json, TEXT("Val"), OutputValueTrue);
  return true;
}