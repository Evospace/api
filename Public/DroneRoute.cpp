#include "DroneRoute.h"

#include "Evospace/JsonHelper.h"
bool FDroneRoute::SerializeJson(TSharedPtr<FJsonObject> json) const {
  json_helper::TrySet(json, "TargetStationName", TargetStationName);
  return true;
}

bool FDroneRoute::DeserializeJson(TSharedPtr<FJsonObject> json) {
  json_helper::TryGet(json, "TargetStationName", TargetStationName);
  return true;
}