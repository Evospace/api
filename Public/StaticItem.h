// Copyright (c) 2017 - 2024, Samsonov Andrey. All Rights Reserved.

#pragma once

#include "../Common.h"
#include "../SerializableJson.h"
#include "Prototype.h"

#include <Engine/World.h>
#include <Internationalization/Text.h>
#include <Math/TransformNonVectorized.h>
#include <Templates/SubclassOf.h>

#include "StaticItem.generated.h"
struct FItemData;
class UUserWidgetSlot;
class AItemLogic;

UENUM(BlueprintType)
enum class EStaticItemType : uint8 {
  Solid,
  Fluid,
  Abstract,
  Count
};

UCLASS(BlueprintType)
/**
 * @brief Static part of every item
 *
 * FItemData {
 *      count: per instance data,
 *      item: static part pointer
 * }
 *
 * Not modifiable in game part of item
 */
class EVOSPACE_API UStaticItem : public UObject, public ISerializableJson {
  GENERATED_BODY()

  public:
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  AItemLogic *SpawnLogicItemDeffered(UWorld *world,
                                     const FTransform &transform) const;

  UFUNCTION(BlueprintCallable, BlueprintCosmetic)
  FText GetLabel() const;

  UFUNCTION(BlueprintCallable, BlueprintCosmetic)
  FText GetDescription() const;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  TArray<FKeyTableObject> mLabelParts = {};

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  EStaticItemType mType = EStaticItemType::Solid;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  FKeyTableObject mLabelFormat;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  /**
     * @brief Property. Multiplier for internal item count units
     *
     * When drawing in inventory this multiplier is applying. Showing number =
     * count * multiplier
     *
     * Default value: 1.0
     *
     * @code{.lua}
     * local coal = StaticItem.find("CoalStaticItem")
     * mul = coal.unit_mul
     * coal.unit_mul = 1.0
     * @endcode
     */
  float mUnitMul = 1.f;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  /**
     * @brief Property. Item ico for drawing in inventory
     *
     * Default value: nil
     *
     * @code{.lua}
     * local coal = StaticItem.find("CoalStaticItem")
     * image = coal.image
     * coal.image = image
     * @endcode
     */
  UTexture2D *mImage = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  TSubclassOf<AItemLogic> mItemLogic;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  /**
   * @brief Property. Mesh for rendering item in world (on ground or at
   * conveyor)
   */
  UStaticMesh *mMesh;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  TArray<UMaterialInterface *> mMaterials;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  /**
     * @brief Property. Max count that can be stored in one slot of default
     * inventory
     *
     * Default value: 0
     *
     * @code{.lua}
     * local coal = StaticItem.find("CoalStaticItem")
     * count = coal.max_count
     * coal.max_count = 64
     * @endcode
     */
  int64 mMaxCount = 0;

  // Database page tag
  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  FName mTag = "";

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  FName mCategory = "";

  // Description common keys
  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  TArray<FKeyTableObject> mDescriptionParts = {};

  // Ingame effects color
  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  FLinearColor mColor = FLinearColor(.5, .5, .5, 1);

  // Hidden in survival
  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  /**
   * @brief Set visibility for non creative game
   */
  bool mCraftable = true;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  bool mStackable = true;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  bool mIncomplete = true;

  TSharedPtr<FJsonObject> mLogicJson;

  EVO_LUA_CODEGEN_DB_EX(StaticItem);
  static void lua_reg(lua_State *L);
};
