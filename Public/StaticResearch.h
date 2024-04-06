#pragma once
#include "Evospace/SerializableJson.h"
#include "Evospace/Vector.h"
#include "StaticResearch.generated.h"

class UStaticChapter;
class AMainPlayerController;

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
  TArray<URecipe *> Recipes;
};

UCLASS(BlueprintType)
class EVOSPACE_API UStaticResearch : public UObject, public ISerializableJson {
  GENERATED_BODY()

  public:
  UStaticResearch();

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  TArray<FKeyTableObject> LabelParts = {};

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UTexture2D *Texture = nullptr;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  TArray<UStaticResearch *> RequiredResearches = {};

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  TArray<FName> RequiredResearchesNames = {};

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  FName ChapterName;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UStaticChapter *Chapter;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  FVector2D Position;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  TArray<UInventory *> DataPoints;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UInventory *NewItemsCache;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  TArray<FResearchUnlockLevel> Unlocks;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UInventory *AlsoNewItemsCache;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  FResearchUnlockLevel AlsoUnlocks;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  bool mCompleteByDefault = false;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  int Level = 0;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  FVector2i LevelMinMax;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  EResearchStatus Type;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  bool MainResearch;

  UFUNCTION(BlueprintCallable, BlueprintCosmetic)
  FText GetLabel() const;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  virtual void PostDeserializeJson() override;

  virtual void ApplyToController(AMainPlayerController *apply_to);
};

UCLASS(BlueprintType)
class EVOSPACE_API UStaticResearchBonusInventory : public UStaticResearch {
  GENERATED_BODY()
  public:
  virtual void ApplyToController(AMainPlayerController *apply_to) override;
};

UCLASS(BlueprintType)
class EVOSPACE_API UStaticResearchModifier : public UStaticResearch {
  GENERATED_BODY()
  public:
  virtual void ApplyToController(AMainPlayerController *apply_to) override;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UStaticModifier *mModifier;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  float mBonusValue;
};

UCLASS(BlueprintType)
class EVOSPACE_API UStaticResearchToolUnlock : public UStaticResearch {
  GENERATED_BODY()
  public:
  virtual void ApplyToController(AMainPlayerController *apply_to) override;
};