#pragma once

#include "CoreMinimal.h"
#include "Qr/CommonConverter.h"
#include "Qr/Prototype.h"
#include "Evospace/Shared/Public/StaticCover.h"

#include "StaticCoverSet.generated.h"

/**
 * Static prototype holding a list of allowed UStaticCover designs.
 * JSON: { "Covers": ["/Game/.../StaticCoverFoo", "/Game/.../StaticCoverBar"] }
 */
UCLASS(BlueprintType)
class UStaticCoverSet : public UPrototype {
  using Self = UStaticCoverSet;
  GENERATED_BODY()
  PROTOTYPE_CODEGEN(StaticCoverSet, StaticCoverSet)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UPrototype>("StaticCoverSet") //@class StaticCoverSet : Prototype
      .addProperty("covers", QR_ARRAY_GET_SET(Covers)) //@field StaticCover[]
      .endClass();
  }
  virtual UClass *GetSuperProto() const override { return StaticClass(); }

  public:
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Covers", meta=(ToolTip="Allowed covers for design tool and designable blocks"))
  TArray<UStaticCover *> Covers;
};


