#pragma once

#include "CoreMinimal.h"
#include "Evospace/Item/ItemLogic.h"
#include "Evospace/Shared/Public/StaticCover.h"
#include "DesignToolItemLogic.generated.h"

class UStaticCoverSet;

/**
 * Design Tool item logic. Allows choosing a cover design and applying it to blocks
 * implementing UCoverAttachInterface.
 */
UCLASS()
class ADesignToolItemLogic : public AItemLogic {
  GENERATED_BODY()

  public:
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) const override;

  virtual void ActionPrimaryPressed() override;

  virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

  // Called by design selection dialog to apply selected cover
  UFUNCTION(BlueprintCallable)
  void UpdateDesign(UStaticCover *cover);

  UFUNCTION(BlueprintCallable)
  void SetSelectedCover(UStaticCover *cover) { SelectedCover = cover; }

  UFUNCTION(BlueprintCallable, BlueprintPure)
  UStaticCover *GetSelectedCover() const { return SelectedCover; }

  // Fetch available cover options from a targeted block (for GUI population)
  UFUNCTION(BlueprintCallable, BlueprintPure)
  UStaticCoverSet * GetAvailableCoversFromTarget() const;

  protected:
  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  UStaticCover *SelectedCover = nullptr;
};


