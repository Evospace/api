#pragma once
#include "CoreMinimal.h"
#include "Materials/MaterialInterface.h"
#include "ThirdParty/luabridge/LuaBridge.h"
#include "BlockActor.generated.h"

class UBlockLogic;

UCLASS(BlueprintType)
class ABlockActor : public AActor {
  GENERATED_BODY()
  public:
  UFUNCTION(BlueprintCosmetic, BlueprintNativeEvent, BlueprintCallable)
  FText GetHoverDescription();

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UBlockLogic *mBlockLogic = nullptr;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  UMaterialInterface *HullMaterial = nullptr;

  UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
  void ForceUpdate();

  UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
  void ApplyGraphicsSettings(int32 preset);

  static void lua_reg(lua_State *L) {
    luabridge::getGlobalNamespace(L)
      .deriveClass<ABlockActor, AActor>("BlockActor") //@class BlockActor : Actor
      .addProperty("logic", &ABlockActor::mBlockLogic, false)
      .addProperty("hull_material", &ABlockActor::HullMaterial)
      .endClass();
  }
};
