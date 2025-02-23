#pragma once
#include "Prototype.h"
#include "SourceData.generated.h"

UCLASS(BlueprintType)
class EVOSPACE_API USourceData : public UInstance {
  using Self = USourceData;
  GENERATED_BODY()
  EVO_CODEGEN_INSTANCE(SourceData)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UInstance>("SourceData") //@class SourceData : Instance
      .addProperty("offset", &Self::mOffset) //@field Vec2i source offset from world (0, 0)
      .addProperty("item", &Self::Item) //@field StaticItem item to mine
      .endClass();
  }

  public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int64 mInitialCapacity = 10000;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int64 mExtractedCount = 0;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UStaticItem *Item = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FVector2i mOffset;

  bool SerializeJson(TSharedPtr<FJsonObject> json);
  bool DeserializeJson(TSharedPtr<FJsonObject> json);

  UFUNCTION(BlueprintCallable)
  float CalculateExtractionSpeed() const;

  UFUNCTION(BlueprintCallable)
  FExtractionData ExtractOre(ADimension *dim);
};
