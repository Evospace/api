#pragma once
#include "ExtractionData.h"
#include "Qr/Prototype.h"
#include "Public/LazyGameSessionData.h"
#include "SourceDataExtensions.h"
#include "SourceData.generated.h"

class ADimension;
class UStaticItem;

UCLASS(BlueprintType)
class USourceData : public UInstance {
  using Self = USourceData;
  GENERATED_BODY()
  EVO_CODEGEN_INSTANCE(SourceData)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UInstance>("SourceData") //@class SourceData : Instance
      .addProperty("position", &Self::Position) //@field Vec2i source position in block coordinates
      .addProperty("item", &Self::Item) //@field StaticItem item to mine
      .addProperty("active_miners", &Self::ActiveMiners) //@field integer number of miners currently extracting
      .addProperty("infinite_ore", &Self::InfiniteOre) //@field boolean
      .endClass();
  }

  public:
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  int32 ActiveMiners = 0;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  const UStaticItem *Item = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  const UStaticProp *Prop = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FVector2i Position;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool InfiniteOre = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int32 Yield = 300;

  bool SerializeJson(TSharedPtr<FJsonObject> json) const;
  bool DeserializeJson(TSharedPtr<FJsonObject> json);

  UFUNCTION(BlueprintCallable)
  int32 GetExtractionSpeed() const;

  UFUNCTION(BlueprintCallable)
  int32 GetTotalYield() const;

  UFUNCTION(BlueprintCallable)
  FExtractionData ExtractOre(int64 count);

  UFUNCTION(BlueprintCallable)
  FExtractionData ExtractOreForMiner(UObject *Miner, int64 count);

  UFUNCTION(BlueprintCallable)
  int32 GetMinerExtractionSpeed(UObject *Miner) const;

  UFUNCTION(BlueprintCallable)
  void OnMinerStart(UObject *Miner);

  UFUNCTION(BlueprintCallable)
  void OnMinerStop(UObject *Miner);

  private:
  void RecalculateMinerAllocations();

  UPROPERTY()
  TArray<FSourceMinerAllocation> MinerAllocations;

  FLazyGameSession GameSessionCache;
};
