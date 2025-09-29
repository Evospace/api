#include "SurfaceDefinition.h"
#include "Public/RegionMap.h"
#include "Qr/JsonHelperCommon.h"

bool USurfaceDefinition::SerializeJson(TSharedPtr<FJsonObject> json) {
  json_helper::TrySet(json, "GeneratorName", GeneratorName);
  json_helper::TrySerialize(json, "RegionMap", RegionMap);
  return true;
}

bool USurfaceDefinition::DeserializeJson(TSharedPtr<FJsonObject> json) {
  json_helper::TryGet(json, "GeneratorName", GeneratorName);
  json_helper::TryDeserialize(json, "RegionMap", RegionMap);
  return true;
}

void USurfaceDefinition::Initialize() {
  RegionMap = NewObject<URegionMap>();
  RegionMap->Initialize(this);
}