// Copyright (c) 2017 - 2022, Samsonov Andrey. All Rights Reserved.

#pragma once

#include "Evospace/Blocks/BlockDeserializeLegacyLuaState.h"
#include "Evospace/Common.h"
#include "Evospace/Item/ItemLogic.h"
#include "Evospace/SerializableJson.h"
#include "Evospace/Shared/Core/prototype.h"

#include <Engine/World.h>
#include <Internationalization/Text.h>
#include <Math/TransformNonVectorized.h>
#include <Templates/SubclassOf.h>

#include "item.generated.h"

struct FItemData;
class UUserWidgetSlot;
class AItemLogic;

UCLASS(BlueprintType)
/**
 * @brief Static part of every item
 *
 * Item Instance {
 *      per instance data,
 *      static part pointer
 * }
 *
 * Not modifiable part of item
 */
class UItem : public UPrototype, public ISerializableJson {
    GENERATED_BODY()

    // Lua api
  public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    /**
     * @brief Property. Item ico for drawing in inventory
     *
     * Default value: nil
     *
     * @code{.lua}
     * image = static_item.image
     * static_item.image = image
     * @endcode
     */
    UTexture2D *image = nullptr;

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
     * mul = static_item.unit_mul
     * static_item.unit_mul = 1.0
     * @endcode
     */
    float unit_mul = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    /**
     * @brief Property. Max count that can be stored in one slot of default
     * inventory
     *
     * Default value: 0
     *
     * @code{.lua}
     * count = static_item.max_count
     * static_item.max_count = 64
     * @endcode
     */
    int64 max_count = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    /**
     * @brief Property. Mesh for rendering item in world (on ground or at
     * conveyor)
     */
    UStaticMesh *mesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    /**
     * @brief Set visibility for non creative game
     */
    bool craftable = true;

    // Engine code
  public:
    virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

    AItemLogic *
    SpawnLogicItemDeffered(UWorld *world, const FTransform &transform) const;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FKeyTableObject> mLabelParts = {};

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FKeyTableObject mLabelFormat;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<AItemLogic> mItemLogic;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<UMaterialInterface *> mMaterials;

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

    TSharedPtr<FJsonObject> mLogicJson;

    UFUNCTION(BlueprintCallable, BlueprintCosmetic)
    FText GetLabel() const;

    UFUNCTION(BlueprintCallable, BlueprintCosmetic)
    FText GetDescription() const;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool mSolid = true;

    EVO_LUA_CODEGEN(UItem, Item);
    static std::function<void(lua_State *)> GetRegisterLambda();
};
EVO_REGISTER_STATIC(UItem, StaticItem);