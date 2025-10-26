#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CoverAttachInterface.generated.h"

class UStaticCover;

UINTERFACE(NotBlueprintable)
class UCoverAttachInterface : public UInterface {
  GENERATED_BODY()
};

class ICoverAttachInterface {
  GENERATED_BODY()

  public:
  UFUNCTION(BlueprintCallable)
  virtual bool AttachCover(UStaticCover *cover) PURE_VIRTUAL(UCoverAttachInterface::AttachCover, return false;);

  UFUNCTION(BlueprintCallable)
  virtual bool DetachCover() PURE_VIRTUAL(UCoverAttachInterface::DetachCover, return false;);

  UFUNCTION(BlueprintCallable)
  virtual UStaticCover *GetAttachedCover() const PURE_VIRTUAL(UCoverAttachInterface::GetAttachedCover, return nullptr;);
};


