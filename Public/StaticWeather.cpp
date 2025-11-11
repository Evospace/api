#include "StaticWeather.h"  
#include "Qr/JsonHelperCommon.h"

bool UStaticWeather::DeserializeJson(TSharedPtr<FJsonObject> json) {
  json_helper::TryGet(json, TEXT("Cloudiness01"), Cloudiness01);
  json_helper::TryGet(json, TEXT("Precipitation01"), Precipitation01);
  json_helper::TryGet(json, TEXT("Fog01"), Fog01);
  json_helper::TryGet(json, TEXT("Storminess01"), Storminess01);
  json_helper::TryGet(json, TEXT("WindSpeed"), WindSpeed);
  json_helper::TryGet(json, TEXT("MinDurationSeconds"), MinDurationSeconds);
  json_helper::TryGet(json, TEXT("MaxDurationSeconds"), MaxDurationSeconds);
  
  Cloudiness01 = FMath::Clamp(Cloudiness01, 0.0f, 1.0f);
  Precipitation01 = FMath::Clamp(Precipitation01, 0.0f, 1.0f);
  Fog01 = FMath::Clamp(Fog01, 0.0f, 1.0f);
  Storminess01 = FMath::Clamp(Storminess01, 0.0f, 1.0f);
  WindSpeed = FMath::Max(0.0f, WindSpeed);

  return true;
}