#pragma once

#include "CoreMinimal.h"
#include "Qr/CommonConverter.h"
#include "Qr/Prototype.h"
#include "CarveSettings.h"
#include "SurfaceDefinition.generated.h"

class URegionMap;

UCLASS(BlueprintType)
class USurfaceDefinition : public UInstance {
  EVO_CODEGEN_INSTANCE(SurfaceDefinition);
  using Self = USurfaceDefinition;
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UInstance>("SurfaceDefinition") //@class SurfaceDefinition : Instance
      .addProperty("region_map", &Self::RegionMap) //@field URegionMap
      .addProperty("generator_name", QR_NAME_GET_SET(GeneratorName)) //@field string
      .endClass();
  }
  GENERATED_BODY()

  public:
  UPROPERTY(BlueprintReadOnly, EditAnywhere)
  URegionMap *RegionMap;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  FName GeneratorName;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldGen|Caves")
  FCarveNoiseSettings CarveSettings;

  UFUNCTION(BlueprintCallable)
  void Initialize();

  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) const override;
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
};