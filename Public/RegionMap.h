// Copyright (c) 2017 - 2022, Samsonov Andrey. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ExtractionData.h"
#include "MapStructure.h"
#include "Evospace/CoordinameMinimal.h"
#include "Evospace/Vector.h"
#include "Evospace/SerializableJson.h"
#include "Evospace/Misc/AssetOwner.h"
#include "Public/Prototype.h"
#include "ThirdParty/luabridge/LuaBridge.h"
#include "UObject/Object.h"
#include "RegionMap.generated.h"

class UEvoRegion;
class ADimension;
class UStaticItem;
class UTexture2D;

UCLASS(BlueprintType)
class EVOSPACE_API USourceData : public UObject {
  GENERATED_BODY()
  public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int64 mInitialCapacity = 10000;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int64 mExtractedCount = 0;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UStaticItem *mItem = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FVector2i mOffset;

  bool SerializeJson(TSharedPtr<FJsonObject> json);
  bool DeserializeJson(TSharedPtr<FJsonObject> json);

  UFUNCTION(BlueprintCallable)
  float CalculateExtractionSpeed() const;

  UFUNCTION(BlueprintCallable)
  FExtractionData ExtractOre(ADimension *dim);
};

UCLASS(BlueprintType)
/**
 * Map generation script object with size and bytecode
 */
class EVOSPACE_API UStaticStructure : public UPrototype {
  GENERATED_BODY()
  EVO_OWNER(StaticStructure)
  EVO_CODEGEN_DB(StaticStructure, StaticStructure)
  virtual void lua_reg(lua_State *L) const {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UStaticStructure, UPrototype>("StaticStructure") //@class StaticStructure : Prototype
      .addProperty("generate", &UStaticStructure::mGenerage) //@field function
      .addProperty("size", &UStaticStructure::mSize) //@field Vec2i
      .endClass();
  }

  public:
  bool SerializeJson(TSharedPtr<FJsonObject> json);
  bool DeserializeJson(TSharedPtr<FJsonObject> json);

  std::optional<luabridge::LuaRef> mGenerage;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  FVector2i mSize;

  virtual void Release() override;
};

UCLASS(BlueprintType)
class EVOSPACE_API UEvoRegion : public UObject {
  GENERATED_BODY()
  public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<USourceData *> mSources;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UTexture2D *mTexture;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UTexture2D *mTextureHeight;

  UFUNCTION(BlueprintCallable, BlueprintPure)
  static UTexture2D *GetFastnoiseTexture(UPARAM(ref) const TArray<float> &array, bool smooth = false);

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<FMapStructure> mMapStructures;

  bool SerializeJson(TSharedPtr<FJsonObject> json);
  bool DeserializeJson(TSharedPtr<FJsonObject> json);

  UTexture2D *CreateAtlasTexture(int32 Width, int32 Height);

  void AddStructure(const FMapStructure &st);

  static void lua_reg(lua_State *L) {
    FExtractionData::lua_reg(L);
    luabridge::getGlobalNamespace(L)
      .beginClass<UEvoRegion>("Region")
      .addFunction("add_structure", &UEvoRegion::AddStructure)
      .endClass();
  }
};
/**
 * 
 */
UCLASS(BlueprintType)
class EVOSPACE_API URegionMap : public UObject, public ISerializableJson {
  GENERATED_BODY()
  public:
  static constexpr int32 gridSize = 256;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TMap<FVector2i, UEvoRegion *> Regions;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<FExtractionData> AllResources;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<int32> ResourceGeneratedCount;

  UFUNCTION(BlueprintCallable)
  float GetGridSize() const { return gridSize; }

  UFUNCTION(BlueprintCallable)
  void Reset() {
    Regions.Empty();
  }

  static FVector2i WorldBlockToGrid(const Vec3i &pos);

  USourceData *FindSource(const Vec3i &bpos);

  USourceData *FindSource(const FVector &wpos);

  UFUNCTION(BlueprintCallable)
  USourceData *FindSourceWrapper(const FVector3i &grid);

  void AddResource(const FExtractionData &ed);

  UFUNCTION(BlueprintCallable)
  UEvoRegion *GetRegion(const FVector2i &grid);

  UEvoRegion *FindRegion(const Vec2i &grid);

  UFUNCTION(BlueprintCallable)
  UEvoRegion *FindRegionWrapper(const FVector2i &grid);

  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  static void lua_reg(lua_State *L) {
    FExtractionData::lua_reg(L);
    FMapStructure::lua_reg(L);
    luabridge::getGlobalNamespace(L)
      .beginClass<URegionMap>("RegionMap")
      .addFunction("add_resource", &URegionMap::AddResource)
      .addFunction("get_region", &URegionMap::GetRegion)
      .addFunction("find_source", &URegionMap::FindSourceWrapper)
      .addFunction("find_region", &URegionMap::FindRegionWrapper)
      .addFunction("has_region", [](URegionMap &self, const FVector2i &pos) { return self.FindRegion(pos) != nullptr; })
      .addStaticFunction("world_block_to_grid", &URegionMap::WorldBlockToGrid)
      .endClass();
  }
};