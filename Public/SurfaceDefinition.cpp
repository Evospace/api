#include "SurfaceDefinition.h"
#include "Public/RegionMap.h"
#include "Qr/JsonHelperCommon.h"

bool USurfaceDefinition::SerializeJson(TSharedPtr<FJsonObject> json) {
  json_helper::TrySerialize(json, "RegionMap", RegionMap);
  json_helper::TrySet(json, "GeneratorName", GeneratorName);
  return true;
}

bool USurfaceDefinition::DeserializeJson(TSharedPtr<FJsonObject> json) {
  json_helper::TryDeserialize(json, "RegionMap", RegionMap);
  json_helper::TryGet(json, "GeneratorName", GeneratorName);
  return true;
}

void USurfaceDefinition::Initialize() {
  RegionMap = NewObject<URegionMap>();
  RegionMap->Initialize(this);
}