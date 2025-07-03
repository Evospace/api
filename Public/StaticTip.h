#pragma once
#include "Qr/Prototype.h"
#include "Evospace/Common.h"
#include "StaticTip.generated.h"

class UAutosizeInventory;
class UDB;

UCLASS(BlueprintType)
/**
 * 
 */
class UStaticTip : public UPrototype {
  GENERATED_BODY()
  public:
  UStaticTip();

  UPROPERTY(BlueprintReadOnly, EditAnywhere)
  FLoc mLabel;

  UPROPERTY(BlueprintReadOnly, EditAnywhere)
  FLoc mDescription;

  UPROPERTY(BlueprintReadOnly, EditAnywhere)
  UAutosizeInventory *mContextInventory;

  UPROPERTY(BlueprintReadOnly, EditAnywhere)
  UTexture2D *mImage;

  std::string mImagePath;

  UFUNCTION(BlueprintCallable)
  void LoadImagePath();

  void AddContext(const UStaticItem *item);
  void ClearContext();

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  EVO_CODEGEN_DB(StaticTip, StaticTip)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UStaticTip, UPrototype>("StaticTip") //@class StaticTip : Prototype
      .addProperty("label", &UStaticTip::mLabel)
      .addProperty("description", &UStaticTip::mDescription)
      .addProperty("image", &UStaticTip::mImagePath)
      .addFunction("add_context", &UStaticTip::AddContext)
      .addFunction("clear_context", &UStaticTip::ClearContext)
      .addProperty("context", &UStaticTip::mContextInventory)
      .endClass();
  }
  virtual UClass *GetSuperProto() const override {
    return StaticClass();
  }
};
