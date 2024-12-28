
// Copyright (c) 2017 - 2024, Samsonov Andrey. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Evospace/SerializableJson.h"
#include "Evospace/Misc/AssetOwner.h"
#include "Public/Prototype.h"

#include "StaticObject.generated.h"

class ADimension;
class UStaticItem;
class AActor;

USTRUCT(BlueprintType)
/**
 * 
 */
struct FStaticBlockMinable {
  GENERATED_BODY()

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  bool Minable = true;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  UStaticItem *Result = nullptr;

  bool DeserializeJson(TSharedPtr<FJsonObject> json);
};

UCLASS(Abstract)
/**
 * 
 */
class EVOSPACE_API UStaticObject : public UPrototype {
  EVO_OWNER(StaticObject)
  EVO_CODEGEN_DB(StaticObject, StaticObject)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UStaticObject, UPrototype>("StaticObject") //class: StaticObject, parent: Prototype
      .addProperty("item", &UStaticObject::mStaticItem) //field: StaticItem
      .endClass();
  }
  GENERATED_BODY()

  public:
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  UPROPERTY(BlueprintReadWrite)
  UStaticItem *mStaticItem;

  UPROPERTY(BlueprintReadOnly, EditAnywhere)
  FStaticBlockMinable mMinable = {};

  UPROPERTY(BlueprintReadOnly)
  FQuat mDefaultRotation = FQuat::Identity;

  UPROPERTY(BlueprintReadOnly)
  bool mSurface = false;

  virtual AActor *CreateSelector() const { return nullptr; };
};
