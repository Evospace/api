#pragma once
#include "Prototype.h"
#include "Evospace/ISearchable.h"
#include "Evospace/SerializableJson.h"
#include "Evospace/Misc/AssetOwner.h"

#include <Evospace/Common.h>
#include "StaticResearch.generated.h"

class AMainPlayerController;
class UInventory;

UCLASS(BlueprintType)
class EVOSPACE_API UOldResearch : public UObject, public ISerializableJson {
  GENERATED_BODY()

  public:
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  TArray<UStaticItem *> mItems;
  TArray<int64> mValues;
};

UENUM()
enum class EResearchStatus : uint8 {
  Opened,
  CanEnqueue,
  Closed,
  Complete,
  Restricted
};

UCLASS(BlueprintType, Abstract)
class EVOSPACE_API UStaticResearch : public UPrototype, public ISearchable {
  GENERATED_BODY()
  using Self = UStaticResearch;
  EVO_CODEGEN_DB(StaticResearch, StaticResearch)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      //@comment Research base class
      .deriveClass<Self, UPrototype>("StaticResearch") //@class StaticResearch : Prototype
      .endClass();
  }
  virtual UClass *GetSuperProto() const override {
    return StaticClass();
  }

  public:
  UStaticResearch();

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  TArray<FKeyTableObject> LabelParts = {};

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  TArray<FKeyTableObject> DescriptionParts = {};

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UInventory *DataPoints;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  int mLevel = 0;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  TArray<UStaticResearch *> RequiredResearch = {};

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  bool mIsUpgrade = false;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  bool MainResearch = false;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  UTexture2D *Texture = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  EResearchStatus Type;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  bool mCompleteByDefault = false;

  virtual void ComputeSearchMetadata() const override;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  virtual void ApplyToController(AMainPlayerController *apply_to, int32 level);

  UFUNCTION(BlueprintCallable, BlueprintCosmetic)
  FText GetLabel() const;

  UFUNCTION(BlueprintCallable, BlueprintCosmetic)
  virtual FText GetDescription() const;

  UFUNCTION(BlueprintCallable, BlueprintCosmetic)
  virtual UTexture2D *GetTexture() const;
};