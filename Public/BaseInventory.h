// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "InventoryAccess.h"
#include "Evospace/Item/InventoryWidget.h"
#include "ItemData.h"
#include "Qr/Prototype.h"

#include <Templates/Function.h>
#include <Templates/SubclassOf.h>

#include "BaseInventory.generated.h"
class UInventoryFilter;
class UBaseInventory;
class UStaticItem;

UCLASS(Abstract, BlueprintType)
class UBaseInventory : public UInventoryAccess {
  GENERATED_BODY()
  EVO_CODEGEN_INSTANCE(BaseInventory)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UBaseInventory, UInventoryAccess>("BaseInventory") //@class BaseInventory : InventoryAccess
      .endClass();
  }

  UBaseInventory();

  void Empty();

  UFUNCTION(BlueprintCallable)
  void RemoveAllSlots() override;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) const override;

  bool Equal(const UBaseInventory *other) const;

  virtual int64 _AddWithLimit(const FItemData &other, bool arm = false) override;

  public:
  virtual TSubclassOf<UBaseInventoryWidgetBase> GetWidgetClass() const override;

  UFUNCTION(BlueprintCallable)
  virtual void SetWidgetClass(TSubclassOf<UInventoryWidgetBase> widget_class);

  virtual int64 GetVersion() const override { return Version; }

  void SetFilter(UInventoryFilter *filter);

  virtual UInventoryFilter *GetFilter() const override;

  virtual int32 _Find(const UStaticItem *item) const override;

  virtual TArray<int32> _FindAll(const UStaticItem *item) const override;

  virtual const FItemData &_SafeGet(int32 index) const override;

  UFUNCTION(BlueprintCallable)
  virtual bool IsEmpty() const override;

  virtual int64 _Sum(const UStaticItem *item) const override;
  virtual FItemData &_GetMut(int32 index) override;
  virtual const FItemData &_Get(int32 index) const override;
  virtual int64 GetCapacity(int32 index) const override;
  virtual int32 _Num() const override;
  virtual int64 _Add(const FItemData &other) override;
  virtual int64 _Add(int32 i, const FItemData &other) override;

  virtual int64 _Sub(const FItemData &other) override;
  virtual int64 _Sub(int32 i, const FItemData &other) override;

  TFunction<void(FItemData &)> OnAdd;
  TFunction<void(FItemData &)> OnRemove;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int64 Capacity = INDEX_NONE;

  // Performance optimization: Version counter for change detection
  // Increments whenever inventory contents change
  UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  int64 Version = 0;

  protected:
  bool CheckFilter(const FItemData &data);

  void ClearSlot(int32 index);

  UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  TArray<FItemData> Data;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  UInventoryFilter *Filter;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  TSubclassOf<UBaseInventoryWidgetBase> WidgetClass;

  bool AutoSize = false;
};
