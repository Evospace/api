#pragma once
#include "Prototype.h"
#include "RegionLayer.generated.h"

class UStaticItem;
class USourceData;

UCLASS(BlueprintType)
class EVOSPACE_API URegionLayer : public UInstance {
  using Self = URegionLayer;
  GENERATED_BODY()
  EVO_CODEGEN_INSTANCE(RegionLayer)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UInstance>("RegionLayer") //@class RegionLayer : Instance
      .addProperty("item", &Self::Item) //@field StaticItem item to mine
      .addProperty("initial_capacity", &Self::InitialCapacity) //@field integer initial source capacity
      .addProperty("extracted_count", &Self::ExtractedCount) //@field integer count of items already extracted from the source
      .endClass();
  }
public:

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int64 InitialCapacity = 10000;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int64 ExtractedCount = 0;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  const UStaticItem *Item;

  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
};