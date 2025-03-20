// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
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
  using Self = UStaticBlock;
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UStaticObject>("StaticBlock") //@class StaticBlock : StaticObject
      .addProperty("logic", &Self::mBlockLogic) //@field Class
      .addProperty("actor", &Self::mActorClass) //@field Class
      .addProperty("selector", &Self::mSelectorClass) //@field Class
      .addProperty("tesselator", &Self::Tesselator) //@field Tesselator
      .addProperty("sub_blocks", &Self::Positions)
      .addProperty(
        "replace_tag", [](const Self *self) -> std::string { return TCHAR_TO_UTF8(*self->ReplaceTag.ToString()); }, [](Self *self, const std::string &s) { self->ReplaceTag = UTF8_TO_TCHAR(s.data()); }) //@field string
      .addProperty("color_side", &Self::mColorSide) //@field Vector
      .addProperty("color_top", &Self::mColorTop) //@field Vector
      .addProperty("tier", &Self::Tier) //@field integer
      .addProperty("level", &Self::Level) //@field integer
      .addProperty("lua", &Self::Table) //@field table
      .endClass();
  }
  GENERATED_BODY()

  public:
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  UPROPERTY(BlueprintReadOnly)
  UTesselator *Tesselator;

  UPROPERTY(BlueprintReadOnly)
  TSubclassOf<UBlockLogic> mBlockLogic = nullptr;

  UPROPERTY(BlueprintReadOnly)
  UClass *mActorClass = nullptr;

  UPROPERTY(BlueprintReadOnly)
  bool NoActorRenderable = false;

  UPROPERTY(BlueprintReadOnly)
  UClass *mSelectorClass = nullptr;

  virtual ABlockActor *SpawnActorAndLuaDeferred(ADimension *world, UBlockLogic *bloc_logic, const FTransform &tr) const;

  UFUNCTION(BlueprintCallable, BlueprintCosmetic)
  FString GetLuaTooltip(UBlockLogic *s) const;

  std::optional<luabridge::LuaRef> Table;

  virtual void Release() override;

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
