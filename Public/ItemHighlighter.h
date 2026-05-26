#pragma once
#include "CoreMinimal.h"
#include "ItemHighlighter.generated.h"

class UStaticItem;

USTRUCT(BlueprintType)
struct FItemHighlighter {
  GENERATED_BODY()

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  TArray<const UStaticItem *> SearchItems;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  TArray<const UStaticItem *> HoveredItem;

  uint32 Generation = 0;

  public:
  void SetSearchItems(const TArray<const UStaticItem *> &InItems) {
    SearchItems = InItems;
    ++Generation;
  }

  void SetHoveredItem(const UStaticItem *InItem) {
    HoveredItem.Empty();
    if (InItem) {
      HoveredItem.Add(InItem);
    }
    ++Generation;
  }

  const TArray<const UStaticItem *> &GetActiveHighlighted() const {
    if (SearchItems.IsEmpty()) {
      return HoveredItem;
    }
    return SearchItems;
  }
};
