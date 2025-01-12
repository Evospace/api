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
class EVOSPACE_API UStaticBlock : public UStaticObject {
  EVO_OWNED(StaticBlock, StaticObject)
  EVO_CODEGEN_DB(StaticBlock, StaticBlock);
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UStaticBlock, UStaticObject>("StaticBlock") //class: StaticBlock, parent: StaticObject
      .addProperty("logic", &UStaticBlock::mBlockLogic) //field: Class
      .addProperty("actor", &UStaticBlock::mActorClass) //field: Class
      .addProperty("selector", &UStaticBlock::mSelectorClass) //field: Class
      .addProperty("tesselator", &UStaticBlock::mTesselator) //field: Tesselator
      .addProperty("sub_blocks", &UStaticBlock::Positions)
      .addProperty("replace_tag", &UStaticBlock::ReplaceTag) //field: string
      .addProperty("color_side", &UStaticBlock::mColorSide) //field: Vector
      .addProperty("color_top", &UStaticBlock::mColorTop) //field: Vector
      .addProperty("tier", &UStaticBlock::Tier) //field: integer
      .addProperty("level", &UStaticBlock::Level) //field: integer
      .addProperty("lua", &UStaticBlock::Table) //field: table
      .endClass();
  }
  GENERATED_BODY()

  public:
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;

  const UTesselator *GetTesselator() const;

  UPROPERTY(BlueprintReadOnly)
  UTesselator *mTesselator;

  UPROPERTY(BlueprintReadOnly)
  TSubclassOf<UBlockLogic> mBlockLogic = nullptr;

  UPROPERTY(BlueprintReadOnly)
  UClass *mActorClass = nullptr;

  UPROPERTY(BlueprintReadOnly)
  UClass *mSelectorClass = nullptr;

  virtual ABlockActor *SpawnActorAndLuaDeferred(ADimension *world, UBlockLogic *bloc_logic, const FTransform &tr) const;

  UFUNCTION(BlueprintCallable, BlueprintCosmetic)
  FString GetLuaTooltip(UBlockLogic * s) const;

  std::optional<luabridge::LuaRef> Table;

  virtual void LuaCleanup() override;

  UPROPERTY(BlueprintReadOnly)
  FVector mColorSide = FVector(1);

  UPROPERTY(BlueprintReadOnly)
  FVector mColorTop = FVector(1);

  const TArray<FVector3i> &GetPositions() const;
  TArray<FVector3i> Positions;

  FName ReplaceTag;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  int32 Tier = 0;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  int32 Level = 0;

  virtual UBlockLogic *SpawnBlockDeferred(ADimension *dim, const FTransform &tr, const Vec3i &bpos) const;

  virtual UPartBlockLogic *SpawnPart(ADimension *world, const FTransform &tr, const Vec3i &bpos, UBlockLogic *parent) const;
};
