#include "MapObjectManager.h"
#include "Public/BlockLogic.h"
#include "Public/StaticItem.h"
#include "Public/MainGameInstance.h"
#include "Public/RegionMap.h"
#include "Public/EvoRegion.h"
#include "Public/Dimension.h"
#include "Public/SurfaceDefinition.h"

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

void UMapObjectManager::ReportBuilt(UBlockLogic *Block) {
  if (!Block) return;
  auto sd = Block->GetDim()->SurfaceDefinition;

  const FVector3i wb = Block->GetWorldPosition();
  const FVector2i grid = URegionMap::WorldBlockToGrid(wb);
  UEvoRegion *region = sd->RegionMap->FindRegion(grid);
  if (!region) return;

  if (!region->TextureOverlay) {
    LOG(WARN_LL) << "MapObjectManager::ReportBuilt No overlay texture";
    return;
  }

  const int32 pixelX = wb.X - grid.X * URegionMap::gridSize;
  const int32 pixelY = wb.Y - grid.Y * URegionMap::gridSize;
  MarkOverlayPixel(region, pixelX, pixelY, FColor(170, 170, 170, 255));
}

void UMapObjectManager::MarkOverlayPixel(UEvoRegion *Region, int32 X, int32 Y, const FColor &Color) {
  if (!Region || !Region->TextureOverlay) return;

  UTexture2D *tex = Region->TextureOverlay;
  FTexture2DMipMap &Mip = tex->GetPlatformData()->Mips[0];
  const int32 Width = tex->GetSizeX();
  const int32 Height = tex->GetSizeY();
  if (X < 0 || Y < 0 || X >= Width || Y >= Height) return;

  void *Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
  FColor *Pixels = static_cast<FColor *>(Data);
  Pixels[Y * Width + X] = Color;
  Mip.BulkData.Unlock();
  tex->UpdateResource();
}
