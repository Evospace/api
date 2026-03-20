#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Containers/Array.h"
#include "Containers/Map.h"
#include "Containers/Set.h"
#include <Containers/Ticker.h>
#include "MapObjectManager.generated.h"

/**
 * Central registry for map-renderable objects.
 * Objects implement IMapObjectInterface and register/unregister here.
 */
class UStaticItem;
class UBlockLogic;
class UEvoRegion;

UCLASS(BlueprintType)
class UMapObjectManager : public UObject {
  GENERATED_BODY()

  public:
  UFUNCTION(BlueprintCallable)
  void Register(UBlockLogic *Block, const UStaticItem *Item);

  UFUNCTION(BlueprintCallable)
  void Unregister(UBlockLogic *Block, const UStaticItem *Item);

  UFUNCTION(BlueprintCallable)
  void UnregisterAll(UBlockLogic *Block);

  const TMap<const UStaticItem *, TArray<UBlockLogic *>> &GetBuckets() const { return ItemToBlocks; }
  const TArray<UBlockLogic *> *GetForItem(const UStaticItem *Item) const { return ItemToBlocks.Find(Item); }

  // Minimap overlay update API
  UFUNCTION(BlueprintCallable)
  void ReportBuilt(UBlockLogic *Block);
  void BeginOverlayBatch();
  void EndOverlayBatch();

  private:
  // Not a UPROPERTY: nested containers of weak pointers are not reflected/serialized and we don't need GC tracking
  // here
  TMap<const UStaticItem *, TArray<UBlockLogic *>> ItemToBlocks;
  TSet<UEvoRegion *> DirtyOverlayRegions;
  int32 OverlayBatchDepth = 0;
  bool bOverlayFlushScheduled = false;
  FTSTicker::FDelegateHandle OverlayFlushTickerHandle;

  void ScheduleOverlayFlush();
  bool OnOverlayFlushTick(float DeltaTime);
  void FlushDirtyOverlayRegions();
  static void MarkOverlayPixel(UEvoRegion *Region, int32 X, int32 Y, const FColor &Color, bool bWritePixel);
};
