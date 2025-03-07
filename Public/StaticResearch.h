#pragma once
#include "Prototype.h"
#include "Evospace/SerializableJson.h"
#include "Evospace/Vector.h"
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
  Closed,
  Complete,
  Restricted
};

USTRUCT(BlueprintType)
struct EVOSPACE_API FResearchUnlock {
  GENERATED_BODY()

  public:
  FName Dictionary;
  FName Recipe;
};

USTRUCT(BlueprintType)
struct EVOSPACE_API FResearchUnlockLevel {
  GENERATED_BODY()

  public:
  TArray<FResearchUnlock> Unlocks;

  UPROPERTY()
  TArray<class URecipe *> Recipes;
};

UCLASS(BlueprintType, Abstract)
class EVOSPACE_API UStaticResearchBase : public UPrototype {
  GENERATED_BODY()
  public:
  UStaticResearchBase();

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  TArray<FKeyTableObject> LabelParts = {};

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  TArray<FKeyTableObject> DescriptionParts = {};

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UInventory *DataPoints;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  int mLevel = 0;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  TArray<FName> RequiredResearchNames = {};

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  TArray<UStaticResearchBase *> RequiredResearch = {};

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

  virtual FString GetSearchMetaString() const;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  virtual void ApplyToController(AMainPlayerController *apply_to, int32 level);

  virtual void PostDeserializeJson() override;

  UFUNCTION(BlueprintCallable, BlueprintCosmetic)
  FText GetLabel() const;

  UFUNCTION(BlueprintCallable, BlueprintCosmetic)
  virtual FText GetDescription() const;

  UFUNCTION(BlueprintCallable, BlueprintCosmetic)
  virtual UTexture2D *GetTexture() const;

  EVO_OWNER(StaticResearchBase)
  EVO_CODEGEN_DB(StaticResearchBase, StaticResearchBase)
  virtual void lua_reg(lua_State *L) const override {}
};

UCLASS(BlueprintType)
class EVOSPACE_API UStaticResearch : public UStaticResearchBase {
  GENERATED_BODY()

  public:
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  TArray<class URecipe *> RecipeUnlocks;
  std::unique_ptr<FResearchUnlockLevel> RecipeUnlocksTemp;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  FVector2i LevelMinMax;

  virtual void PostDeserializeJson() override;

  virtual FString GetSearchMetaString() const;

  virtual void ApplyToController(AMainPlayerController *apply_to, int32 level) override;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  virtual FText GetDescription() const override;

  virtual UTexture2D *GetTexture() const override;

  EVO_OWNED(StaticResearch, StaticResearchBase)
  EVO_CODEGEN_DB(StaticResearch, StaticResearchBase)
  virtual void lua_reg(lua_State *L) const override {}
};

UCLASS(BlueprintType)
class EVOSPACE_API UStaticResearchBonusInventory : public UStaticResearchBase {
  GENERATED_BODY()
  public:
  virtual void ApplyToController(AMainPlayerController *apply_to, int32 level) override;

  EVO_OWNED(StaticResearchBonusInventory, StaticResearchBase)
  EVO_CODEGEN_DB(StaticResearchBonusInventory, StaticResearchBase)
  virtual void lua_reg(lua_State *L) const override {}
};

UCLASS(BlueprintType)
class EVOSPACE_API UStaticResearchModifier : public UStaticResearchBase {
  GENERATED_BODY()
  public:
  virtual void ApplyToController(AMainPlayerController *apply_to, int32 level) override;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  class UStaticModifier *mModifier = nullptr;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  float mBonusValue;

  EVO_OWNED(StaticResearchModifier, StaticResearchBase)
  EVO_CODEGEN_DB(StaticResearchModifier, StaticResearchBase)
  virtual void lua_reg(lua_State *L) const override {}
};

UCLASS(BlueprintType)
class EVOSPACE_API UStaticResearchEfficiency : public UStaticResearchBase {
  GENERATED_BODY()
  public:
  virtual void ApplyToController(AMainPlayerController *apply_to, int32 level) override;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  FName mRecipeDictionary;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  FName mRecipe;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  float mBonusValue;

  EVO_OWNED(StaticResearchModifier, StaticResearchBase)
  EVO_CODEGEN_DB(StaticResearchModifier, StaticResearchBase)
  virtual void lua_reg(lua_State *L) const override {}
};

UCLASS(BlueprintType)
class EVOSPACE_API UStaticResearchToolUnlock : public UStaticResearchBase {
  GENERATED_BODY()
  public:
  virtual void ApplyToController(AMainPlayerController *apply_to, int32 level) override;

  EVO_OWNED(StaticResearchToolUnlock, StaticResearchBase)
  EVO_CODEGEN_DB(StaticResearchToolUnlock, StaticResearchBase)
  virtual void lua_reg(lua_State *L) const override {}
};