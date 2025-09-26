#pragma once

#include "CoreMinimal.h"
#include "Qr/Prototype.h"
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
      .endClass();
  }
  GENERATED_BODY()

  public:
  UPROPERTY(BlueprintReadOnly, EditAnywhere)
  URegionMap *RegionMap;

  UFUNCTION(BlueprintCallable)
  void Initialize();
  
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
};