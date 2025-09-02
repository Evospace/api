#pragma once
#include "InventoryFilter.h"
#include "Qr/Prototype.h"

#include "InventoryWhiteFilter.generated.h"

UCLASS(BlueprintType)
class UInventoryWhiteFilter : public UInventoryFilter {
  GENERATED_BODY()
  EVO_CODEGEN_INSTANCE(InventoryWhiteFilter)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UInventoryWhiteFilter, UInstance>("InventoryWhiteFilter") //@class InventoryWhiteFilter : InventoryFilter
      .endClass();
  }

  public:
  virtual bool Check(const FItemData &data) override;

  void AddItem(const UStaticItem *item);
  void Empty();

  UPROPERTY(VisibleAnywhere)
  TSet<const UStaticItem *> Items;
};