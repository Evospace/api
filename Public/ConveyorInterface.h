#pragma once

#include "Qr/CoordinameMinimal.h"
#include "ConveyorInterface.generated.h"


class ULogicContext;class UInventoryAccess;
struct RItemInstancingHandle;

UINTERFACE(BlueprintType)
class UConveyorInterface : public UInterface {
  GENERATED_BODY()
};

class IConveyorInterface {
  GENERATED_BODY()
  public:
  virtual void SetPushedSide(const Vec3i &side, RItemInstancingHandle &&item) = 0;

  UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
  UInventoryAccess *GetInputAccess() const;

  virtual UInventoryAccess *GetInputAccess_Implementation() const = 0;

  UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
  UInventoryAccess *GetOutputAccess() const;

  virtual UInventoryAccess *GetOutputAccess_Implementation() const = 0;
};