#pragma once

#include "CoreMinimal.h"
#include "Public/BlockLogic.h"
#include "Evospace/Shared/Public/StaticCover.h"
#include "Evospace/Shared/Public/StaticCoverSet.h"
#include "Evospace/Shared/Public/DesignToolSupportInterface.h"
#include "Evospace/Misc/CoverWrapper.h"

#include "DesignableCoverBlockLogic.generated.h"

/**
 * Cover-capable block that allows changing its center cover design at runtime.
 */
UCLASS(BlueprintType)
class UDesignableCoverBlockLogic : public UBlockLogic, public IDesignToolSupportInterface {
  GENERATED_BODY()
  using Self = UDesignableCoverBlockLogic;
  EVO_CODEGEN_INSTANCE(DesignableCoverBlockLogic)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UBlockLogic>("DesignableCoverBlockLogic") //@class DesignableCoverBlockLogic : BlockLogic
      .addProperty("cover_set", &Self::CoverSet) //@field StaticCoverSet* CoverSet
      .endClass();
  }
  public:
  UDesignableCoverBlockLogic() = default;

  // Rendering lifecycle
  virtual void SetRenderable(class AColumn *sector) override;
  virtual void RemoveActorOrRenderable() override;

  // Settings serialization (stores current cover)
  virtual void SaveSettings(TSharedPtr<class FJsonObject> json, class AMainPlayerController *mpc = nullptr) const override;
  virtual void LoadSettings(TSharedPtr<class FJsonObject> json, class AMainPlayerController *mpc = nullptr) override;

  // Design tool support
  virtual UStaticCoverSet *GetDesignOptions() const override { return CoverSet; }
  virtual bool SelectCover(UStaticCover *cover) override;
  virtual bool SetCoverColor(const FLinearColor &color, int32 colorIndex = 0) override {
    if (colorIndex == 0) {
      AppliedCoverColor = color;
    }
#ifdef EVOSPACE_COVERS_RENDERING
    if (!AppliedCoverInstance.IsEmpty()) {
      return AppliedCoverInstance.SetColor(color, colorIndex);
    }
#endif
    return true;
  }
  
  // Actions
  virtual bool HasAction() const override { return true; }
  virtual void OnAction(const FHitResult &hit, const Vec3i &side, AItemLogic *item) override;
  
  virtual void BlockBeginPlay() override;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Covers")
  UStaticCoverSet *CoverSet = nullptr;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Covers")
  UStaticCover *AppliedCover = nullptr;

  RCoverWrapper AppliedCoverInstance;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Covers")
  FLinearColor AppliedCoverColor = FLinearColor::White;
};


