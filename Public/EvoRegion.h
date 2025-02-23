#pragma once
#include "MapStructure.h"
#include "Prototype.h"
#include "EvoRegion.generated.h"

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

  void AddStructure(const FMapStructure &mapStructure);
  void AddSource(USourceData *sourceData);
};