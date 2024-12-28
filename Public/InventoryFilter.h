// Copyright (c) 2017 - 2024, Samsonov Andrey. All Rights Reserved.
#pragma once

#include "Containers/Array.h"
#include "CoreMinimal.h"
#include "ItemData.h"

#include "InventoryFilter.generated.h"
class UInventoryAccess;
class UStaticItem;

UCLASS(Abstract, BlueprintType)
class EVOSPACE_API UInventoryFilter : public UObject {
  GENERATED_BODY()

  public:
  virtual bool Check(const FItemData &data);
};

UCLASS(BlueprintType)
class EVOSPACE_API UInventoryWhiteFilter : public UInventoryFilter {
  GENERATED_BODY()

  public:
  virtual bool Check(const FItemData &data) override;

  void AddItem(const UStaticItem *item);
  void Empty();

  UPROPERTY()
  TSet<const UStaticItem *> mItems;
};

UCLASS(BlueprintType)
class EVOSPACE_API UInventoryBlackFilter : public UInventoryFilter {
  GENERATED_BODY()

  public:
  virtual bool Check(const FItemData &data) override;
};

UCLASS(BlueprintType)
class EVOSPACE_API UInventoryInventoryFilter : public UInventoryFilter {
  GENERATED_BODY()

  public:
  virtual bool Check(const FItemData &data) override;

  void SetInventory(UInventoryAccess *inv);
  UInventoryAccess *GetInventory() const;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  bool mIsWhite = true;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  bool mIsFiltering = true;

  UPROPERTY()
  UInventoryAccess *mInventory;
};
