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

/**
 * 
 */
UCLASS(BlueprintType)
class EVOSPACE_API URegionMap : public UInstance {
  GENERATED_BODY()
  EVO_CODEGEN_INSTANCE(RegionMap)
  virtual void lua_reg(lua_State *L) const override {
    FExtractionData::lua_reg(L);
    FMapStructure::lua_reg(L);
    luabridge::getGlobalNamespace(L)
      .deriveClass<URegionMap, UInstance>("RegionMap") //@class RegionMap : Instance
      .addFunction("add_resource", &URegionMap::AddResource)
      //direct:
      //---Get Region by its position
      //---@param spos Vec2i position in RegionMap grid
      //---@return Region
      //function RegionMap:get_region(spos) end
      .addFunction("get_region", &URegionMap::GetRegion)
      .addFunction("find_source", &URegionMap::FindSourceWrapper)
      .addFunction("find_region", &URegionMap::FindRegionWrapper)
      .addFunction("has_region", [](URegionMap &self, const FVector2i &pos) { return self.FindRegion(pos) != nullptr; })
      //direct:
      //---Convert Block World position to RegionMap grid cell that contains this position
      //---@param bpos Vec3i
      //--- @return Vec2i RegionMap grid position
      //function RegionMap.world_block_to_grid(bpos) end
      .addStaticFunction("world_block_to_grid", &URegionMap::WorldBlockToGrid)
      .endClass();
  }

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
};