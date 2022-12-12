
#pragma once
#include "CoreMinimal.h"
#include <string>
#include "Evospace/Vector.h"
#include "Evospace/Common.h"
#include "Evospace/SerializableJson.h"
#include "static_research.generated.h"

class AMainPlayerController;
class UStaticChapter;
class URecipe;
class UStaticItem;
class UInventory;
class UInternalInventory;

UCLASS(BlueprintType)
class EVOSPACE_API UOldResearch : public UObject, public ISerializableJson {
    GENERATED_BODY()

public:
    virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;
    virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

    TArray<UStaticItem *> mItems;
    TArray<int64> mValues;
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
    TArray<const URecipe *> Recipes;
};

UENUM()
enum class EResearchStatus : uint8 { Opened, Closed, Complete, Restricted };

class UStaticResearch;

UCLASS(BlueprintType)
class EVOSPACE_API UStaticChapter : public UObject, public ISerializableJson {
    GENERATED_BODY()

  public:

    std::string name;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FKeyTableObject> LabelParts = {};

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FKeyTableObject> DescriptionParts = {};

    UFUNCTION(BlueprintCallable, BlueprintCosmetic)
    FText GetLabel() const;

    UFUNCTION(BlueprintCallable, BlueprintCosmetic)
    FText GetDescription() const;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<UStaticResearch *> Researches;

    virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

    virtual void PostDeserializeJson() override;
};

UCLASS(BlueprintType)
/**
 * @brief 
 */
class EVOSPACE_API UStaticResearch : public UObject, public ISerializableJson {
    GENERATED_BODY()

    // Lua api
  public:

    /**
     * @brief Object name in database
     */
    std::string name;
    
    UTexture2D *get_image() const { return Texture; }
    void set_image(UTexture2D *val) { Texture = val; }
    
    Vec2i get_position() const { return Position; }
    void set_position(const Vec2i & val) { Position = FVector2D(val.X, val.Y); }

  public:
    
    UStaticResearch();

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FKeyTableObject> LabelParts = {};

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UTexture2D *Texture = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UInternalInventory *Collect;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UInternalInventory *CollectDefault;

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
    UStaticResearchBonusInventory();

    virtual void ApplyToController(AMainPlayerController *apply_to) override;
};