#pragma once
#include "MapStructure.h"
#include "Prototype.h"
#include "EvoRegion.generated.h"

class URegionLayer;
class USourceData;

UCLASS(BlueprintType)
class EVOSPACE_API UEvoRegion : public UInstance {
  using URegion = UEvoRegion;
  using Self = UEvoRegion;
  GENERATED_BODY()
  EVO_CODEGEN_INSTANCE(Region)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UInstance>("Region") //@class Region : Instance
      .addFunction("add_structure", &Self::AddStructure)
      //direct:
      //---Add source to this Region
      //---@param spos SourceData
      //function Region:add_source(spos) end
      .addFunction("add_source", &Self::AddSource)
      .endClass();
  }

  public:
  UEvoRegion();
  
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  URegionLayer *OilLayer;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  URegionLayer *FertileLayer;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<USourceData *> Sources;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UTexture2D *Texture;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UTexture2D *TextureHeight;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FVector2i RegionPos;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<FMapStructure> MapStructures;

  UFUNCTION(BlueprintCallable, BlueprintPure)
  static UTexture2D *GetFastnoiseTexture(UPARAM(ref) const TArray<float> &array, bool smooth = false);

  void Initialize(const FVector2i &pos, int32 gridSize);

  bool SerializeJson(TSharedPtr<FJsonObject> json);
  bool DeserializeJson(TSharedPtr<FJsonObject> json);

  UTexture2D *CreateAtlasTexture(int32 Width, int32 Height);

  void AddStructure(const FMapStructure &mapStructure);
  void AddSource(USourceData *sourceData);
};