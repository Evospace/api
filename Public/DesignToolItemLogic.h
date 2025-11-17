#pragma once

#include "CoreMinimal.h"
#include "Evospace/Item/ItemLogic.h"
#include "Evospace/Shared/Public/StaticCover.h"
#include "DesignToolItemLogic.generated.h"

class UStaticCoverSet;
class IDesignToolSupportInterface;

/**
 * Design Tool item logic. Allows choosing a cover design and applying it to blocks
 * implementing UCoverAttachInterface.
 */
UCLASS()
class ADesignToolItemLogic : public AItemLogic {
  GENERATED_BODY()

  public:
  virtual void ActionPrimaryPressed() override;

  virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

  // Called by design selection dialog to apply selected cover
  UFUNCTION(BlueprintCallable)
  void UpdateDesign(UStaticCover *cover);

  // Called by UI to apply color to the current block's cover instance
  UFUNCTION(BlueprintCallable)
  void UpdateDesignColor(const FLinearColor &color, int32 colorIndex);

  // Fetch available cover options from a targeted block (for GUI population)
  UFUNCTION(BlueprintCallable, BlueprintPure)
  UStaticCoverSet *GetAvailableCoversFromTarget() const;

  private:
  IDesignToolSupportInterface *DesignToolSupportInterface = nullptr;
};
