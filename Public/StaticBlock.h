// Copyright (c) 2017 - 2024, Samsonov Andrey. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Evospace/CoordinateSystem.h"
#include "Evospace/Vector.h"
#include "Evospace/Misc/AssetOwner.h"
#include "Public/BlockLogic.h"
#include "Public/Prototype.h"
#include "Public/StaticObject.h"

#include <string>
#include <Chaos/Array.h>
#include <UObject/ScriptInterface.h>

#include "StaticBlock.generated.h"

class ABlockActor;
class UTesselator;
class UBlockLogic;
class UPartBlockLogic;
class ADimension;

UCLASS(BlueprintType)
/**
 * Static Block
 */
class EVOSPACE_API UStaticBlock : public UStaticObject {
  EVO_OWNED(StaticBlock, StaticObject)
  EVO_CODEGEN_DB(StaticBlock, StaticBlock);
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UStaticBlock, UStaticObject>("StaticBlock")
      .addProperty("logic", &UStaticBlock::mBlockLogic)
      .addProperty("actor", &UStaticBlock::mActorClass)
      .addProperty("selector", &UStaticBlock::mSelectorClass)
      .addProperty("tesselator", &UStaticBlock::mTesselator)
      .addProperty("sub_blocks", &UStaticBlock::Positions)
      .addProperty("replace_tag", &UStaticBlock::ReplaceTag)
      .addProperty("color_side", &UStaticBlock::mColorSide)
      .addProperty("color_top", &UStaticBlock::mColorTop)
      .addProperty("tier", &UStaticBlock::Tier)
      .addProperty("level", &UStaticBlock::Level)
      .addProperty("lua", &UStaticBlock::Table)
      .endClass();
  }
  GENERATED_BODY()

  public:
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;

  const UTesselator *GetTesselator() const;

  UPROPERTY(BlueprintReadOnly)
  /**
   * Tesselator
   */
  UTesselator *mTesselator;

  UPROPERTY(BlueprintReadOnly)
  /**
   * Block Logic
   */
  TSubclassOf<UBlockLogic> mBlockLogic = nullptr;

  UPROPERTY(BlueprintReadOnly)
  /**
   * Actor Class
   */
  UClass *mActorClass = nullptr;

  UPROPERTY(BlueprintReadOnly)
  UClass *mSelectorClass = nullptr;

  virtual ABlockActor *SpawnActorAndLuaDeferred(ADimension *world, UBlockLogic *bloc_logic, const FTransform &tr) const;

  //std::string mActorBytecode;
  //std::string mBlockBytecode;

  std::optional<luabridge::LuaRef> Table;

  virtual void LuaCleanup() override;

  UPROPERTY(BlueprintReadOnly)
  /**
   * Color side
   */
  FVector mColorSide = FVector(1);

  UPROPERTY(BlueprintReadOnly)
  /**
   * Color top
   */
  FVector mColorTop = FVector(1);

  const TArray<FVector3i> &GetPositions() const;
  TArray<FVector3i> Positions;

  /**
   * Replace tag
   */
  FName ReplaceTag;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  /**
   * Tier
   */
  int32 Tier = 0;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  /**
   * Level
   */
  int32 Level = 0;

  virtual UBlockLogic *SpawnBlockDeferred(ADimension *dim, const FTransform &tr, const Vec3i &bpos) const;

  virtual UPartBlockLogic *SpawnPart(ADimension *world, const FTransform &tr, const Vec3i &bpos, UBlockLogic *parent) const;
};
