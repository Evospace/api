#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DesignToolSupportInterface.generated.h"

class UStaticCover;
class UStaticCoverSet;

UINTERFACE(NotBlueprintable)
class UDesignToolSupportInterface : public UInterface {
  GENERATED_BODY()
};

class IDesignToolSupportInterface {
  GENERATED_BODY()

  public:
  // Return available cover designs for GUI and validation
  UFUNCTION(BlueprintCallable)
  virtual UStaticCoverSet *GetDesignOptions() const PURE_VIRTUAL(IDesignToolSupportInterface::GetDesignOptions, return nullptr;);

  // Select and apply a specific design (cover)
  UFUNCTION(BlueprintCallable)
  virtual bool SelectCover(UStaticCover *cover) PURE_VIRTUAL(IDesignToolSupportInterface::SelectCover, return false;);
};


