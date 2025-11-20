#pragma once

#include "CoreMinimal.h"
#include "InventoryReader.h"

#include "UObject/ObjectMacros.h"
#include "ItemData.h"
#include "ItemMap.generated.h"

class UInventoryReader;
class UInventoryAccess;


UCLASS(BlueprintType)
class UItemMap : public UObject {
  GENERATED_BODY()
  public:
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  TMap<const UStaticItem *, int64> Map;

  UFUNCTION(BlueprintCallable)
  int64 Get(const UStaticItem *Item) const;

  UFUNCTION(BlueprintCallable)
  void Set(const UStaticItem *Item, int64 Value);

  void Add(const FItemData &data);

  UFUNCTION(BlueprintCallable)
  void Add(const UStaticItem *Item, int64 Delta);

  UFUNCTION(BlueprintCallable)
  void FromInventory(const UInventoryReader *reader);

  // Performance optimization: FromInventory with version checking
  void FromInventoryOptimized(const UInventoryAccess *reader, int64 &LastInventoryVersion);

  UFUNCTION(BlueprintCallable)
  void Subtract(const UStaticItem *Item, int64 Delta);

  UFUNCTION(BlueprintCallable)
  bool Has(const UStaticItem *Item, int64 MinAmount = 1) const;

  UFUNCTION(BlueprintCallable)
  void Clear();
};
