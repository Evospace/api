// Copyright (c) 2017 - 2022, Samsonov Andrey. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Prototype.h"
#include "Evospace/Common.h"
#include "Evospace/SerializableJson.h"
#include "Evospace/Misc/AssetOwner.h"
#include "UObject/Object.h"
#include "StaticModifier.generated.h"

UCLASS(BlueprintType)
class UStaticModifier : public UPrototype {
  GENERATED_BODY()
  public:
  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  UTexture2D *mImage;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  FKeyTableObject mLabel;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  FKeyTableObject mDescription;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  EVO_CODEGEN_DB(StaticModifier, StaticModifier)
  virtual void lua_reg(lua_State *L) const override {}
  virtual UClass *GetSuperProto() const override {
    return StaticClass();
  }
};
