#pragma once
#include "ExtractionData.h"
#include "Qr/Prototype.h"
#include "SourceData.generated.h"

class ADimension;
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
  const UStaticItem *Item = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  const UStaticProp *Prop = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FVector2i Position;

  bool SerializeJson(TSharedPtr<FJsonObject> json);
  bool DeserializeJson(TSharedPtr<FJsonObject> json);

  UFUNCTION(BlueprintCallable)
  static int32 CalculateExtractionSpeed(int64 extracted, int64 initial);

  UFUNCTION(BlueprintCallable)
  FExtractionData ExtractOre(ADimension *dim);
};
