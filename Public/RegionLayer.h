#pragma once
#include "Dimension.h"
#include "ExtractionData.h"
#include "Qr/Prototype.h"
#include "RegionLayer.generated.h"

class UStaticItem;
class USourceData;

USTRUCT(BlueprintType)
struct FSubregionData final {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int64 InitialCapacity = 10000;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int64 ExtractedCount = 0;

  static constexpr int Subdivision = 4;
  static constexpr FVector2i SubdivisionSize = { Subdivision, Subdivision };

  bool SerializeJson(TSharedPtr<FJsonObject> json) const;
  bool DeserializeJson(TSharedPtr<FJsonObject> json);
};

UCLASS(BlueprintType)
class EVOSPACE_API URegionLayer : public UInstance {
  using Self = URegionLayer;
  GENERATED_BODY()
  EVO_CODEGEN_INSTANCE(RegionLayer)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UInstance>("RegionLayer") //@class RegionLayer : Instance
      .addProperty("item", &Self::Item) //@field StaticItem item to mine
      .endClass();
  }

  public:
  URegionLayer();

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int64 MaxCapacity = 1000;

  UPROPERTY(VisibleAnywhere)
  TArray<FSubregionData> Data;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  const UStaticItem *Item = nullptr;

  FExtractionData ExtractItem(const ADimension *dim, const FVector2i &sr);

  const FSubregionData &GetSubregion(const FVector2i &sr) const;
  FSubregionData &GetSubregion(const FVector2i &sr);

  UFUNCTION(BlueprintCosmetic, BlueprintPure)
  float GetVisualPercent(const FVector2i &pos) const;

  static int32 CalculateRegen(const FSubregionData &sub);

  UFUNCTION(BlueprintCosmetic, BlueprintPure)
  float GetRegen(const FVector2i &pos) const;

  UFUNCTION(BlueprintCallable)
  int64 GetExtractedCount(const FVector2i &pos) const;

  UFUNCTION(BlueprintCallable)
  int64 GetInitialCapacity(const FVector2i &pos) const;

  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
};