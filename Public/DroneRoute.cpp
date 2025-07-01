#include "DroneRoute.h"

#include "Evospace/JsonHelper.h"
bool FDroneRoute::SerializeJson(TSharedPtr<FJsonObject> json) const {
  json_helper::TrySet(json, "TargetStationID", TargetStationID);
  return true;
}

bool FDroneRoute::DeserializeJson(TSharedPtr<FJsonObject> json) {
  json_helper::TryGet(json, "TargetStationID", TargetStationID);
  return true;
}