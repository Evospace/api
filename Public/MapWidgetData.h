#pragma once

#include "Qr/JsonHelperCommon.h"
#include "Qr/SerializableJson.h"
#include "MapWidgetData.generated.h"

UCLASS(BlueprintType)
class UMapWidgetData : public UObject, public ISerializableJson {
  GENERATED_BODY()
public:
  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  bool OreLayer = true;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  bool FertileLayer = false;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  bool OilLayer = false;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  bool OreNames = false;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  bool OreDescription = false;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  bool PercentText = false;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  bool SpawnPoint = true;

  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
};

inline bool UMapWidgetData::SerializeJson(TSharedPtr<FJsonObject> json) {
  json_helper::TrySet(json, "OreLayer", OreLayer);
  json_helper::TrySet(json, "FertileLayer", FertileLayer);
  json_helper::TrySet(json, "OilLayer", OilLayer);
  json_helper::TrySet(json, "OreNames", OreNames);
  json_helper::TrySet(json, "OreDescription", OreDescription);
  json_helper::TrySet(json, "PercentText", PercentText);
  json_helper::TrySet(json, "SpawnPoint", SpawnPoint);
  return true;
}

inline bool UMapWidgetData::DeserializeJson(TSharedPtr<FJsonObject> json) {
  json_helper::TryGet(json, "OreLayer", OreLayer);
  json_helper::TryGet(json, "FertileLayer", FertileLayer);
  json_helper::TryGet(json, "OilLayer", OilLayer);
  json_helper::TryGet(json, "OreNames", OreNames);
  json_helper::TryGet(json, "OreDescription", OreDescription);
  json_helper::TryGet(json, "PercentText", PercentText);
  json_helper::TryGet(json, "SpawnPoint", SpawnPoint);
  return true;
}