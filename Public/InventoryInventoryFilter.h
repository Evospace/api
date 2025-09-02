#pragma once
#include "InventoryFilter.h"
#include "InventoryInventoryFilter.generated.h"
UCLASS(BlueprintType)
class UInventoryInventoryFilter : public UInventoryFilter {
  using Self = UInventoryInventoryFilter;
  GENERATED_BODY()
  EVO_CODEGEN_INSTANCE(InventoryInventoryFilter)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UInventoryInventoryFilter, UInstance>("InventoryInventoryFilter") //@class InventoryInventoryFilter : InventoryFilter
      .addProperty("inventory", &Self::Inventory) //@field InventoryAccess (nil by default)
      .addProperty("is_filtering", &Self::IsFiltering) //@field boolean is filtering enabled (true by default)
      .addProperty("is_white", &Self::IsWhite) //@field boolean is white filtering enabled (true by default)
      .endClass();
  }

  public:
  virtual bool Check(const FItemData &data) override;

  void SetInventory(UInventoryAccess *inv);
  UInventoryAccess *GetInventory() const;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  bool IsWhite = true;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  bool IsFiltering = true;

  UPROPERTY()
  UInventoryAccess *Inventory;
};