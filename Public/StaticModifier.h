// Copyright (c) 2017 - 2022, Samsonov Andrey. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Qr/Prototype.h"
#include "UObject/Object.h"
#include "Qr/Loc.h"
#include "StaticModifier.generated.h"

UCLASS(BlueprintType)
class UStaticModifier : public UPrototype {
  GENERATED_BODY()
  public:
  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  UTexture2D *mImage;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  FLoc mLabel;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  FLoc mDescription;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  PROTOTYPE_CODEGEN(StaticModifier, StaticModifier)
  virtual void lua_reg(lua_State *L) const override {}
  virtual UClass *GetSuperProto() const override {
    return StaticClass();
  }
};
