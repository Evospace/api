#pragma once
#include "Qr/Prototype.h"
#include "StaticCover.generated.h"

UCLASS(BlueprintType)
class UStaticCover : public UPrototype {
  GENERATED_BODY()
  PROTOTYPE_CODEGEN(StaticCover, StaticCover)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UStaticCover, UPrototype>("StaticCover") //@class StaticCover : Prototype
      .endClass();
  }
  virtual UClass *GetSuperProto() const override { return StaticClass(); }

  public:
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  UPROPERTY(VisibleAnywhere)
  TArray<UMaterialInterface *> Materials;

  UPROPERTY(VisibleAnywhere)
  UStaticMesh *Mesh;

  UPROPERTY(VisibleAnywhere)
  uint8 NumCustomData = 0;

  // Default per-instance colors for covers supporting CustomData
  // If NumCustomData == 3 -> use Colors[0] (R,G,B)
  // If NumCustomData == 6 -> use Colors[0] (R,G,B) and Colors[1] (R,G,B)
  UPROPERTY(VisibleAnywhere)
  TArray<FLinearColor> DefaultColors;

  UPROPERTY(VisibleAnywhere)
  bool NoCollision = false;
};