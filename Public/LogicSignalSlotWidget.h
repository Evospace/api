// Copyright (c) 2017 - 2026, Samsonov Andrei. All Rights Reserved.
#pragma once

#include "Evospace/Item/InventoryWidget.h"
#include "LogicSignalSlotWidget.generated.h"

class USingleSlotInventory;
class UStaticItem;
class UUserWidgetSlot;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLogicSignalSlotChanged);

/** Single-signal picker: inventory access + one UserWidgetSlot. */
UCLASS()
class ULogicSignalSlotWidget : public UDragableInventoryWidgetBase {
  GENERATED_BODY()

  protected:
  virtual void NativeOnInitialized() override;

  public:
  void Setup(UObject *InventoryOuter, FName InventoryName);
  void SyncFromSignal(const UStaticItem *Signal);
  const UStaticItem *ReadSignal() const;

  UPROPERTY(BlueprintAssignable)
  FLogicSignalSlotChanged OnSignalChanged;

  virtual void SetInventoryAccess(UInventoryAccess *inventory) override;
  virtual UInventoryAccess *GetInventoryAccess() override;

  virtual FEventReply OnSlotMouseButtonDown(const FGeometry &MyGeometry, const FPointerEvent &MouseEvent, int32 index) override;
  virtual FEventReply OnSlotMouseButtonUp(const FGeometry &MyGeometry, const FPointerEvent &MouseEvent, int32 index) override;

  private:
  void BuildLayout();
  void NotifySignalChanged();

  UPROPERTY()
  TObjectPtr<USingleSlotInventory> SignalInventory;

  UPROPERTY()
  TObjectPtr<UUserWidgetSlot> SlotWidget;
};
