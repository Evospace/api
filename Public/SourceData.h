#pragma once
#include "ExtractionData.h"
#include "Qr/Prototype.h"
#include "Public/LazyGameSessionData.h"
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
      .addProperty("initial_capacity", &Self::InitialCapacity) //@field integer initial source capacity (ore quantity)
      .addProperty("current_ore", &Self::CurrentOre) //@field integer current remaining ore quantity (clamped to 2% of initial)
      .addProperty("active_miners", &Self::ActiveMiners) //@field integer number of miners currently extracting
      .addProperty("infinite_ore", &Self::InfiniteOre) //@field boolean
      .endClass();
  }

  public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int64 InitialCapacity = 10000;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int64 CurrentOre = 100000;

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

  bool SerializeJson(TSharedPtr<FJsonObject> json) const;
  bool DeserializeJson(TSharedPtr<FJsonObject> json);

  UFUNCTION(BlueprintCallable)
  int32 GetExtractionSpeed() const;

  UFUNCTION(BlueprintCallable)
  int32 GetTotalYield() const;

  UFUNCTION(BlueprintCallable)
  FExtractionData ExtractOre(int64 count);

  UFUNCTION(BlueprintCallable)
  void OnMinerStart();

  UFUNCTION(BlueprintCallable)
  void OnMinerStop();

  private:
  FLazyGameSession GameSessionCache;
};
