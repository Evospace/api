#include "ItemMap.h"

#include "Public/InventoryAccess.h"
#include "Qr/Ensure.h"
#include "Public/BaseInventory.h"

int64 UItemMap::Get(const UStaticItem *Item) const {
  if (const int64 *Val = Map.Find(Item)) {
    return *Val;
  }
  return 0;
}
void UItemMap::Set(const UStaticItem *Item, int64 Value) {
  if (Value == 0) {
    Map.Remove(Item);
  } else {
    Map.Add(Item, Value);
  }
}
void UItemMap::Add(const FItemData &data) {
  int64 &Val = Map.FindOrAdd(data.mItem);
  Val += data.mValue;
  if (Val == 0) {
    Map.Remove(data.mItem);
  }
}
void UItemMap::Add(const UStaticItem *Item, int64 Delta) {
  int64 &Val = Map.FindOrAdd(Item);
  Val += Delta;
  if (Val == 0) {
    Map.Remove(Item);
  }
}
void UItemMap::FromInventory(const UInventoryReader *reader) {
  if (!expect_once(reader, "ItemMap::FromInventory with nullptr InventoryReader"))
    return;

  for (const auto &data : reader->GetSlots()) {
    if (data.mItem != nullptr && data.mValue != 0) {
      Add(data.mItem, data.mValue);
    }
  }
}

void UItemMap::FromInventoryOptimized(const UInventoryAccess *reader, int64 &LastInventoryVersion) {
  if (!expect_once(reader, "ItemMap::FromInventoryOptimized with nullptr InventoryReader"))
    return;

  auto CurrentVersion = reader->GetVersion();

  if (CurrentVersion != LastInventoryVersion) {
    Clear();
    for (const auto &data : reader->GetSlots()) {
      if (data.mItem != nullptr && data.mValue != 0) {
        Add(data.mItem, data.mValue);
      }
    }
    LastInventoryVersion = CurrentVersion;
  }
}
void UItemMap::Subtract(const UStaticItem *Item, int64 Delta) {
  Add(Item, -Delta);
}
bool UItemMap::Has(const UStaticItem *Item, int64 MinAmount) const {
  return Get(Item) >= MinAmount;
}
void UItemMap::Clear() {
  Map.Reset();
}