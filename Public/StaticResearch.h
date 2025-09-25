#pragma once
#include "Qr/Prototype.h"
#include "Qr/ISearchable.h"
#include "Qr/Loc.h"
#include "Qr/SerializableJson.h"

#include <Evospace/Common.h>
#include "StaticResearch.generated.h"

class AMainPlayerController;
class UInventory;

UCLASS(BlueprintType)
class UOldResearch : public UObject, public ISerializableJson {
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
class UStaticResearch : public UPrototype, public ISearchable {
  GENERATED_BODY()
  using Self = UStaticResearch;
  PROTOTYPE_CODEGEN(StaticResearch, StaticResearch)
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
  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  FLoc Label;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  TArray<FLoc> DescriptionParts = {};

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  int64 Complexity = 0;

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