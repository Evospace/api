// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once
#include "ItemData.h"
#include "Qr/Prototype.h"
#include "Qr/ISearchable.h"
#include "Qr/CommonConverter.h"
#include "Qr/Loc.h"

#include <Engine/World.h>
#include <Internationalization/Text.h>
#include <Math/TransformNonVectorized.h>
#include <Templates/SubclassOf.h>

#include "StaticItem.generated.h"

class UStaticBlock;
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
class UStaticItem : public UPrototype, public ISearchable {
  using Self = UStaticItem;
  GENERATED_BODY()
  PROTOTYPE_CODEGEN(StaticItem, StaticItem)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UStaticItem, UPrototype>("StaticItem") //@class StaticItem : Prototype
      .addProperty("image", &Self::Image) //@field Texture Item image in UI
      .addProperty("stack_size", &Self::mStackSize) //@field integer Size of item stack
      .addProperty("tier", &Self::Tier) //@field integer Item unlock tier
      .addProperty("unit_mul", &Self::UnitMul) //@field number multiplier for UI
      .addProperty("mesh", &Self::mMesh) //@field StaticMesh Mesh for item rendering in world
      .addProperty("block", &Self::Block) //@field StaticObject Buildable object pointer for this item
      .addProperty("custom_data", &Self::CustomData) //@field bool Is item instance contains CustomData
      .addProperty("custom_data", &Self::CustomData) //@field bool Is item instance contains CustomData
      .addProperty("logic", &Self::mItemLogic) //@field Class Class for item while in hand
      .addProperty("lua", &Self::Table) //@field table
      .addProperty("tags", &Self::Tags) //@field string
      .addProperty("category", QR_NAME_GET_SET(mCategory)) //@field string In-game database category
      .addProperty("label", &Self::Label) //@field Loc
      .addProperty("description_parts", QR_ARRAY_GET_SET(DescriptionParts)) //@field Loc[]
      .endClass();
  }
  virtual UClass *GetSuperProto() const override {
    return StaticClass();
  }

  public:
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  virtual void ComputeSearchMetadata() const override;

  AItemLogic *SpawnLogicItemDeferred(UWorld *world, const FTransform &transform) const;

  virtual void Release() override;

  UFUNCTION(BlueprintCallable, BlueprintCosmetic)
  FText GetLabel() const;

  UFUNCTION(BlueprintCallable, BlueprintCosmetic)
  FText GetDescription() const;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  FLoc Label = {};

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  EStaticItemType mType = EStaticItemType::Solid;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  FString Tags;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  float UnitMul = 1.f;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  int32 Tier = 0;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  UTexture2D *Image = nullptr;

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
  TArray<FLoc> DescriptionParts = {};

  // Ingame effects color
  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  FLinearColor mColor = FLinearColor(.5, .5, .5, 1);

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  bool mStackable = true;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  int64 mMaxCharge = 0;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  UStaticBlock *Block;

  virtual void MarkIncomplete() override;
};
