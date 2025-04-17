#pragma once
#include "Prototype.h"
#include "SourceData.generated.h"

class UStaticItem;

UCLASS(BlueprintType)
class EVOSPACE_API USourceData : public UInstance {
  using Self = USourceData;
  GENERATED_BODY()
  EVO_CODEGEN_INSTANCE(SourceData)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UInstance>("SourceData") //@class SourceData : Instance
      .addProperty("position", &Self::Position) //@field Vec2i source position in block coordinates
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
  class UStaticProp *Prop = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FVector2i Position;

  bool SerializeJson(TSharedPtr<FJsonObject> json);
  bool DeserializeJson(TSharedPtr<FJsonObject> json);

  UFUNCTION(BlueprintCallable)
  int32 CalculateExtractionSpeed() const;

  UFUNCTION(BlueprintCallable)
  FExtractionData ExtractOre(ADimension *dim);
};
