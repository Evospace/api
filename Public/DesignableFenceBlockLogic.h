#pragma once

#include "CoreMinimal.h"
#include "Public/BlockLogic.h"
#include "Public/StaticCover.h"
#include "Evospace/Misc/CoverWrapper.h"
#include "Evospace/Shared/Public/DesignToolSupportInterface.h"
#include "Evospace/Shared/Public/StaticCoverSet.h"

#include "DesignableFenceBlockLogic.generated.h"

/**
 * Fence block that spawns instanced half covers based on neighbor connections.
 * Center post is always spawned at block center.
 */
UCLASS(BlueprintType)
class UDesignableFenceBlockLogic : public UBlockLogic, public IDesignToolSupportInterface {
  GENERATED_BODY()
  using Self = UDesignableFenceBlockLogic;
  EVO_CODEGEN_INSTANCE(DesignableFenceBlockLogic)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UBlockLogic>("DesignableFenceBlockLogic") //@class DesignableFenceBlockLogic : BlockLogic
      .addProperty("half_cover", &Self::HalfCover) //@field StaticCover
      .addProperty("center_cover", &Self::CenterCover) //@field StaticCover
      .addProperty("cover_set", &Self::CoverSet) //@field StaticCoverSet
      .endClass();
  }

  public:
  UDesignableFenceBlockLogic() = default;

  // Lifecycle
  virtual void BlockBeginPlay() override;
  virtual void SetRenderable(class AColumn *sector) override;
  virtual void RemoveActorOrRenderable() override;

  // Settings serialization (stores selected half-cover and color usage)
  virtual void SaveSettings(TSharedPtr<class FJsonObject> json, class AMainPlayerController *mpc = nullptr) const override;
  virtual void LoadSettings(TSharedPtr<class FJsonObject> json, class AMainPlayerController *mpc = nullptr) override;

  // Design tool support
  virtual UStaticCoverSet *GetDesignOptions() const override { return CoverSet; }
  virtual bool SelectCover(UStaticCover *cover) override;
  virtual bool SetCoverColor(const FLinearColor &color, int32 colorIndex = 0) override;

  // Actions
  virtual bool HasAction() const override { return true; }
  virtual void OnAction(const FHitResult &hit, const Vec3i &side, AItemLogic *item) override;

  // Neighbor changes
  virtual void NeighborBlockAdded(UBlockLogic *block, const Vec3i &pos) override;
  virtual void NeighborBlockRemoved(UBlockLogic *block, const Vec3i &pos) override;

  // Allow free rotation if needed
  virtual Vec3i GetRotationLocks() const override { return { 0, 0, 0 }; }

  // Configurable covers
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fence|Covers")
  UStaticCover *HalfCover = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fence|Covers")
  UStaticCover *CenterCover = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fence|Covers")
  UStaticCoverSet *CoverSet = nullptr;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fence|Covers")
  FLinearColor AppliedCoverColor = FLinearColor::White;

  protected:
  void RebuildInstances(UBlockLogic *exceptNeighbor = nullptr);
  bool ShouldConnectTo(const Vec3i &otherBpos, UBlockLogic *exceptNeighbor) const;

  private:
  RCoverWrapper CenterCoverInstance;
  TArray<RCoverWrapper> HalfCoverInstances;
};
