#pragma once
#include "Dimension.h"
#include "ExtractionData.h"
#include "Qr/Prototype.h"
#include "Public/LazyGameSessionData.h"
#include "RegionLayer.generated.h"

class UStaticItem;
class USourceData;

USTRUCT(BlueprintType)
struct FSubregionData final {
  GENERATED_BODY()

  // Accumulated fertility regeneration bonus provided by gameplay (e.g., fertilizer blocks).
  // This value is consumed and reset during periodic region regeneration updates.
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int32 RegenBoost = 0;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int32 Yield = 300;

  static constexpr int Subdivision = 4;
  static constexpr FVector2i SubdivisionSize = { Subdivision, Subdivision };

  bool SerializeJson(TSharedPtr<FJsonObject> json) const;
  bool DeserializeJson(TSharedPtr<FJsonObject> json);
};

UCLASS(BlueprintType)
class URegionLayer : public UInstance {
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

  UPROPERTY(EditAnywhere)
  TArray<FSubregionData> Data;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  const UStaticItem *Item = nullptr;

  FExtractionData ExtractItem(const FVector2i &sr);

  UFUNCTION(BlueprintPure, BlueprintCallable)
  int32 CalculateExtractionSpeed(const FVector2i &sr) const;

  const FSubregionData &GetSubregion(const FVector2i &sr) const;
  FSubregionData &GetSubregion(const FVector2i &sr);

  UFUNCTION(BlueprintCosmetic, BlueprintPure)
  float GetVisualPercent(const FVector2i &pos) const;

  UFUNCTION(BlueprintCallable)
  int32 GetTotalYield(const FVector2i &sr) const;

  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) const override;
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  private:
  FLazyGameSession GameSessionCache;
};