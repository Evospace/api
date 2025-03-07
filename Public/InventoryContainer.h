// Copyright (c) 2017 - 2024, Samsonov Andrey. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Public/InventoryAccess.h"

#include "InventoryContainer.generated.h"
UCLASS()
class EVOSPACE_API UInventoryContainer : public UInventoryAccess {
  GENERATED_BODY()
  EVO_CODEGEN_INSTANCE(InventoryContainer)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UInventoryContainer, UInventoryAccess>("InventoryContainer") //@class InventoryContainer : InventoryAccess
      //direct:
      //---@field index integer
      //function InventoryContainer:get_access(index) end
      .addFunction("get_access", &UInventoryContainer::GetAccess)
      //direct:
      //---@field inventory InventoryAccess
      //function InventoryContainer:bind(inventory) end
      .addFunction("bind", &UInventoryContainer::BindInventory)
      .endClass();
  }

  virtual TSubclassOf<UBaseInventoryWidgetBase> GetWidgetClass() const override;
  
  virtual int32 _Find(const UStaticItem *item) const override;

  virtual bool IsEmpty() const override;

  virtual int64 _Sum(const UStaticItem *item) const override;

  virtual FItemData &_GetMut(int32 index) override;
  virtual const FItemData &_Get(int32 index) const override;
  
  virtual const FItemData &_SafeGet(int32 index) const override;
  
  virtual int32 _Num() const override;

  virtual void SortKeyAZ() override;
  
  virtual int64 _Add(const FItemData &other) override;
  
  virtual int64 _Add(int32 index, const FItemData &other) override;
  
  virtual int64 _Sub(const FItemData &other) override;
  
  virtual int64 _Sub(int32 index, const FItemData &from) override;

  virtual void TrySetFilter(int32 index, UInventoryFilter *filter) override;

  virtual int64 _AddWithLimit(const FItemData &other, bool by_arm = false) override;

  // 	void BindInventory(UInventoryContainer *container);
  //
  // 	void BindInventory(UBaseInventory *inventory);

  void BindInventory(UInventoryAccess *inventory);

  UInventoryAccess *GetAccess(int32 i) const;

  virtual int64 GetCapacity(int32 index) const override;

  void UnbindAll();

  virtual void SetCanHaveZeroSlot(bool value) override;

  UFUNCTION(BlueprintCallable, BlueprintPure)
  TArray<UInventoryAccess *> &GetList();

  private:
  UPROPERTY(EditAnywhere)
  TArray<UInventoryAccess *> mList;

  std::optional<std::pair<int32, int32>> ListIndex(int32 index) const;

  public:
  int32 InventoryCount();
};
