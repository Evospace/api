#include "LogicExportOption.h"

bool ULogicExportOption::DeserializeJson(TSharedPtr<FJsonObject> json) {
  json_helper::TryFind(json, "Signal", Signal);
  json_helper::TryGet(json, "Enabled", bEnabled);
  json_helper::TryGet(json, "Label", Label);
  json_helper::TryGet(json, "Tooltip", Tooltip);
  return true;
}