#include "MapObjectManager.h"
#include "Public/BlockLogic.h"
#include "Public/StaticItem.h"

void UMapObjectManager::Register(UBlockLogic *Block, const UStaticItem *Item) {
  if (!Block || !Item) return;
  auto &bucket = ItemToBlocks.FindOrAdd(Item);
  if (!bucket.Contains(Block)) {
    bucket.Add(Block);
  }
}

void UMapObjectManager::Unregister(UBlockLogic *Block, const UStaticItem *Item) {
  if (!Block || !Item) return;
  if (auto arr = ItemToBlocks.Find(Item)) {
    arr->Remove(Block);
    if (arr->Num() == 0) {
      ItemToBlocks.Remove(Item);
    }
  }
}

void UMapObjectManager::UnregisterAll(UBlockLogic *Block) {
  if (!Block) return;
  for (auto It = ItemToBlocks.CreateIterator(); It; ++It) {
    auto &arr = It.Value();
    arr.Remove(Block);
    if (arr.Num() == 0) {
      It.RemoveCurrent();
    }
  }
}


