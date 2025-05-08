// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once
#include "ItemData.h"
#include "Prototype.h"
#include "Evospace/Common.h"
#include "Evospace/ISearchable.h"
#include "Evospace/Misc/ArrayConverter.h"

#include "Evospace/Misc/AssetOwner.h"

#include <Engine/World.h>
#include <Internationalization/Text.h>
#include <Math/TransformNonVectorized.h>
#include <Templates/SubclassOf.h>

#include "StaticItem.generated.h"

class UStaticObject;
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
class EVOSPACE_API UStaticItem : public UPrototype, public ISearchable {
  using Self = UStaticItem;
  GENERATED_BODY()
  EVO_OWNER(StaticItem)
  EVO_CODEGEN_DB(StaticItem, StaticItem)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UStaticItem, UPrototype>("StaticItem") //@class StaticItem : Prototype
      .addProperty("image", &Self::mImage) //@field Texture Item image in UI
      .addProperty("stack_size", &Self::mStackSize) //@field integer Size of item stack
      .addProperty("unit_mul", &Self::mUnitMul) //@field number multiplier for UI
      .addProperty("mesh", &Self::mMesh) //@field StaticMesh Mesh for item rendering in world
      .addProperty("object", &Self::Object) //@field StaticObject Buildable object pointer for this item
      .addProperty("custom_data", &Self::CustomData) //@field bool Is item instance contains CustomData
      .addProperty("custom_data", &Self::CustomData) //@field bool Is item instance contains CustomData
      .addProperty("logic", &Self::mItemLogic) //@field Class Class for item while in hand
      .addProperty("lua", &Self::Table) //@field table
      .addProperty(
        "category", [](const Self *self) //@field string In-game database category
        { return std::string(TCHAR_TO_UTF8(*self->mCategory.ToString())); },
        [](Self *self, std::string_view s) { self->mCategory = UTF8_TO_TCHAR(s.data()); })
      .addProperty(
        "label_parts", [](const Self *self) //@field Loc[]
        { return evo::to_vector(self->mLabelParts); },
        [](Self *self, const std::vector<FKeyTableObject> &value) {
          self->mLabelParts = evo::to_array(value);
        })
      .addProperty(
        "description_parts", [](const Self *self) //@field Loc[]
        { return evo::to_vector(self->mDescriptionParts); },
        [](Self *self, const std::vector<FKeyTableObject> &value) {
          self->mDescriptionParts = evo::to_array(value);
        })
      .endClass();
  }

  public:
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  virtual void PostDeserializeJson() override;

  virtual void ComputeSearchMetadata() const override;

  AItemLogic *SpawnLogicItemDeferred(UWorld *world, const FTransform &transform) const;

  virtual void Release() override;

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
  float mUnitMul = 1.f;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  UTexture2D *mImage = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  UClass *mItemLogic;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  UStaticMesh *mMesh;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  bool CustomData;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  TArray<UMaterialInterface *> mMaterials;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  int64 mStackSize = 1;

  // Item database category
  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  FName mCategory = "";

  std::optional<luabridge::LuaRef> Table;

  // Description common keys
  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  TArray<FKeyTableObject> mDescriptionParts = {};

  // Ingame effects color
  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  FLinearColor mColor = FLinearColor(.5, .5, .5, 1);

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  bool mStackable = true;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  int64 mMaxCharge = 0;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  FName ObjectName;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  UStaticObject *Object;

  virtual void MarkIncomplete() override;
};
