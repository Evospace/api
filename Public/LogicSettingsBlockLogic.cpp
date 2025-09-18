#include "LogicSettingsBlockLogic.h"
#include "Public/LogicSettings.h"

ULogicSettings *ULogicSettingsBlockLogic::GetLogicSettings() {
  if (LogicSettings)
    return LogicSettings;
  if (ULogicSettings *init = NewObject<ULogicSettings>(this, TEXT("LogicSettings"))) {
    LogicSettings = init;
    return LogicSettings;
  }
  return nullptr;
}