// Copyright (c) 2017 - 2022, Samsonov Andrey. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ExtractionData.h"
#include "MapStructure.h"
#include "Qr/CoordinameMinimal.h"
#include "Qr/Vector.h"
#include "Qr/Prototype.h"
#include "StaticItem.h"
#include "ThirdParty/luabridge/LuaBridge.h"
#include "UObject/Object.h"
#include "RegionMap.generated.h"

class UWidget;
class UOreGenerator;
class USourceData;
class UEvoRegion;
class ADimension;
class UStaticItem;
class UTexture2D;
class USurfaceDefinition;

UCLASS(BlueprintType)
class UUserMapMarker : public UObject {
  GENERATED_BODY()

  public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FQrVector2i Position;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Label;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FLinearColor Color;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UStaticItem *IconItem;

  bool SerializeJson(TSharedPtr<FJsonObject> json) const;
  bool DeserializeJson(TSharedPtr<FJsonObject> json);
};

UCLASS(BlueprintType)
class URegionMap : public UInstance {
  GENERATED_BODY()
  EVO_CODEGEN_INSTANCE(RegionMap)
  virtual void lua_reg(lua_State *L) const override {
    FExtractionData::lua_reg(L);
    FMapStructure::lua_reg(L);
    luabridge::getGlobalNamespace(L)
      .deriveClass<URegionMap, UInstance>("RegionMap") //@class RegionMap : Instance
      // direct:
      //---Delete all regions
      // function RegionMap:reset() end
      .addFunction("reset", &URegionMap::Reset)
      // direct:
      //---Get Region by its position
      //---@param spos Vec2i position in RegionMap grid
      //---@return Region
      // function RegionMap:get_region(spos) end
      .addFunction("get_region", &URegionMap::GetRegion)
      // direct:
      //---Looking for existing Region with given sector position
      //---@param spos Vec2i position in RegionMap grid
      //---@return Region
      // function RegionMap:find_region(spos) end
      .addFunction("find_region", &URegionMap::FindRegion)
      // direct:
      //---Is region exists
      //---@param pos Vec2i region position
      //---@return boolean
      // function RegionMap:has_region(pos) end
      .addFunction("has_region",
                   [](URegionMap &self, const FQrVector2i &pos) { return self.FindRegion(pos) != nullptr; })
      // direct:
      //---Convert Block World position to RegionMap grid cell that contains this position
      //---@param bpos Vec3i
      //--- @return Vec2i RegionMap grid position
      // function RegionMap.world_block_to_grid(bpos) end
      .addStaticFunction("world_block_to_grid", &URegionMap::WorldBlockToGrid)
      .endClass();
  }

  public:
  static constexpr int32 gridSize = 256;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TMap<FQrVector2i, UEvoRegion *> Regions;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UOreGenerator *OreGenerator;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<UUserMapMarker *> UserMarkers;

  UFUNCTION(BlueprintCallable)
  float GetGridSize() const { return gridSize; }

  UFUNCTION(BlueprintCallable)
  void Initialize(USurfaceDefinition *surfaceDefinition);

  UFUNCTION(BlueprintCallable)
  void Reset() { Regions.Empty(); }

  UPROPERTY()
  TWeakObjectPtr<USurfaceDefinition> SurfaceDefinition = nullptr;

  UPROPERTY()
  USourceData *StoneSource;

  UPROPERTY()
  USourceData *WaterSource;

  UFUNCTION(BlueprintCallable)
  static FQrVector2i SectorToGrid(const FQrVector2i &pos);

  UFUNCTION(BlueprintCallable)
  static FQrVector2i WorldBlockToGrid(const FQrVector3i &pos);

  UFUNCTION(BlueprintCallable)
  static FQrVector2i WorldBlockToSubregion(const FQrVector3i &pos);

  USourceData *FindSource(const Vec3i &wbpos, EStaticItemType AllowedItemType);

  UFUNCTION(BlueprintCallable)
  UEvoRegion *GetRegion(const FQrVector2i &region_pos);

  UFUNCTION(BlueprintCallable)
  UEvoRegion *FindRegion(const FQrVector2i &grid);

  UFUNCTION(BlueprintCallable)
  UEvoRegion *FindRegionAt(const FQrVector3i &position);
  UEvoRegion *GetRegionAt(const FQrVector3i &position);

  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) const override;
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  bool PrepareTextures();

  // User marker API
  UFUNCTION(BlueprintCallable)
  UUserMapMarker *AddMarker(const FQrVector2i &Position, const FString &Label, const FLinearColor &Color, UStaticItem *IconItem);

  UFUNCTION(BlueprintCallable)
  bool UpdateMarker(UUserMapMarker *Marker,
                    const FString &Label,
                    const FLinearColor &Color,
                    UStaticItem *IconItem,
                    const FQrVector2i &Position);

  UFUNCTION(BlueprintCallable)
  bool RemoveMarker(UUserMapMarker *Marker);

  UFUNCTION(BlueprintCallable, BlueprintPure)
  const TArray<UUserMapMarker *> &GetMarkers() const { return UserMarkers; }
};