#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CoverAttachInterface.generated.h"

class UStaticCover;

UINTERFACE(MinimalAPI, Blueprintable)
class UCoverAttachInterface : public UInterface {
  GENERATED_BODY()
};

class ICoverAttachInterface {
  GENERATED_BODY()

  public:
  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool AttachCover(UStaticCover *cover);
  virtual bool AttachCover_Implementation(UStaticCover *cover) = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool DetachCover();
  virtual bool DetachCover_Implementation() = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  bool HasCover() const;
  virtual bool HasCover_Implementation() const = 0;

  UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
  UStaticCover *GetAttachedCover() const;
  virtual UStaticCover *GetAttachedCover_Implementation() const = 0;
};


