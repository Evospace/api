// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Evospace/CoordinateSystem.h"
#include "Evospace/Vector.h"
#include "Evospace/Misc/AssetOwner.h"
#include "Evospace/Misc/EvoConverter.h"
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
  using Self = UStaticBlock;
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UStaticObject>("StaticBlock") //@class StaticBlock : StaticObject
      .addProperty("logic", &Self::mBlockLogic) //@field Class
      .addProperty("actor", &Self::mActorClass) //@field Class
      .addProperty("selector", &Self::mSelectorClass) //@field Class
      .addProperty("tesselator", &Self::Tesselator) //@field Tesselator
      .addProperty("sub_blocks", EVO_ARRAY_GET_SET(Positions)) //@field Vec3i[]
      .addProperty("replace_tag", EVO_NAME_GET_SET(ReplaceTag)) //@field string
      .addProperty("color_side", &Self::mColorSide) //@field Vec3
      .addProperty("color_top", &Self::mColorTop) //@field Vec3
      .addProperty("tier", &Self::Tier) //@field integer
      .addProperty("level", &Self::Level) //@field integer
      .addProperty("lua", &Self::Table) //@field table
      .endClass();
  }
  GENERATED_BODY()

  public:
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UTesselator *Tesselator;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UClass *mBlockLogic = nullptr;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UClass *mActorClass = nullptr;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  bool NoActorRenderable = false;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UClass *mSelectorClass = nullptr;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  FVector mColorSide = FVector(1);

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  FVector mColorTop = FVector(1);

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  TArray<FVector3i> Positions;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  FName ReplaceTag;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  int32 Tier = 0;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  int32 Level = 0;

  virtual ABlockActor *SpawnActorAndLuaDeferred(ADimension *world, UBlockLogic *bloc_logic, const FTransform &tr) const;

  UFUNCTION(BlueprintCallable, BlueprintCosmetic)
  FString GetLuaTooltip(UBlockLogic *s) const;

  std::optional<luabridge::LuaRef> Table;

  virtual void Release() override;

  const TArray<FVector3i> &GetPositions() const;

  virtual UBlockLogic *SpawnBlockDeferred(ADimension *dim, const FTransform &tr, const Vec3i &bpos) const;

  virtual UPartBlockLogic *SpawnPart(ADimension *world, const FTransform &tr, const Vec3i &bpos, UBlockLogic *parent) const;
};
