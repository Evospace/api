// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#pragma once
#include "Prototype.h"
#include "Evospace/Common.h"
#include "Evospace/ISearchable.h"

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
  GENERATED_BODY()
  EVO_OWNER(StaticItem)
  EVO_CODEGEN_DB(StaticItem, StaticItem)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UStaticItem, UPrototype>("StaticItem") //@class StaticItem : Prototype
      .addProperty("image", &UStaticItem::mImage) //@field Texture
      .addProperty("stack_size", &UStaticItem::mStackSize) //@field integer
      .addProperty("unit_mul", &UStaticItem::mUnitMul) //@field number
      .addProperty("mesh", &UStaticItem::mMesh) //@field StaticMesh
      .addProperty("object", &UStaticItem::Object) //@field StaticObject
      .endClass();
  }

  public:
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  virtual void PostDeserializeJson() override;

  virtual void ComputeSearchMetadata() const override;

  AItemLogic *SpawnLogicItemDeferred(UWorld *world, const FTransform &transform) const;

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
  TSubclassOf<AItemLogic> mItemLogic;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  UStaticMesh *mMesh;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  TArray<UMaterialInterface *> mMaterials;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  int64 mStackSize = 0;

  // Item database category
  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  FName mCategory = "";

  // Description common keys
  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  TArray<FKeyTableObject> mDescriptionParts = {};

  // Ingame effects color
  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  FLinearColor mColor = FLinearColor(.5, .5, .5, 1);

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  bool mStackable = true;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  bool mIncomplete = true;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  int64 mMaxCharge = 0;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  FName ObjectName;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  UStaticObject *Object;

  virtual void MarkIncomplete() override;
};
