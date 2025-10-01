#include "SurfaceDefinition.h"
#include "Public/RegionMap.h"
#include "Qr/JsonHelperCommon.h"
#include "Qr/GameInstanceHelper.h"

bool USurfaceDefinition::SerializeJson(TSharedPtr<FJsonObject> json) const {
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
  RegionMap = NewObject<URegionMap>(this, TEXT("RegionMap"));
  RegionMap->Initialize(this);
}