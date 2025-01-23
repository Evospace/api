// Copyright (c) 2017 - 2024, Samsonov Andrey. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "InventoryAccess.h"
#include "Evospace/Item/InventoryWidget.h"
#include "ItemData.h"
#include "Prototype.h"

#include <Templates/Function.h>
#include <Templates/SubclassOf.h>

#include "BaseInventory.generated.h"
class UInventoryFilter;
class UBaseInventory;
class UStaticItem;

UCLASS(Abstract, BlueprintType)
class EVOSPACE_API UBaseInventory : public UInventoryAccess {
  GENERATED_BODY()
  EVO_CODEGEN_INSTANCE(BaseInventory)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UBaseInventory, UInventoryAccess>("BaseInventory") //class: BaseInventory, parent: InventoryAccess
      .endClass();
  }

  UBaseInventory();

  /**
   * Make all slots empty (value = 0, type = nullptr)
   */
  UFUNCTION(BlueprintCallable)
  void Empty();

  UFUNCTION(BlueprintCallable)
  void RemoveAllSlots() override;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;

  bool Equal(const UBaseInventory *other) const;

  virtual int64 _AddWithLimit(const FItemData &other, bool arm = false) override;

  public:
  virtual TSubclassOf<UBaseInventoryWidgetBase> GetWidgetClass() const override;

  UFUNCTION(BlueprintCallable)
  virtual void SetWidgetClass(TSubclassOf<UInventoryWidgetBase> widget_class);

  void SetFilter(UInventoryFilter *filter);

  virtual UInventoryFilter *GetFilter() const override;

  auto LogContext() const {
    auto contextLogger = [this]() {
      if (GetOuter()) {
        return FString("inventory ") + this->GetName() + " from " + GetOuter()->GetName();
      }

      return FString("inventory ") + this->GetName() + " without outer";
    };
    return contextLogger;
  }

  virtual int32 _Min() const override;
  virtual int32 _Max() const override;
  virtual int32 _Find(const UStaticItem *item) const override;
  virtual int32 _FindEmpty() const override;
  virtual int32 _FindNotEmpty() const override;

  virtual const FItemData &_SafeGet(int32 index) const override;

  UFUNCTION(BlueprintCallable)
  virtual bool IsEmpty() const override;

  virtual int64 _Sum(const UStaticItem *item) const override;
  virtual FItemData &_GetMut(int32 index) override;
  virtual const FItemData &_Get(int32 index) const override;
  virtual int64 GetCapacity(const UStaticItem *item = nullptr) const override;
  virtual int64 GetArmCapacity(const UStaticItem *item = nullptr) const override;
  virtual int32 _Num() const override;
  virtual int64 _Add(const FItemData &other) override;
  virtual int64 _Add(int32 index, const FItemData &other) override;

  virtual int64 _Sub(const FItemData &other) override;
  virtual int64 _Sub(int32 index, const FItemData &other) override;

  TFunction<void(FItemData &)> OnAdd;
  TFunction<void(FItemData &)> OnRemove;

  std::optional<int64> Capacity;
  std::optional<int64> ArmCapacity;

  protected:
  bool CheckFilter(const FItemData &data);

  void ClearSlot(int32 index);

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  TArray<FItemData> Data;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  UInventoryFilter *Filter;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  TSubclassOf<UBaseInventoryWidgetBase> WidgetClass;

  bool AutoSize = false;
};
