// Copyright (c) 2017 - 2024, Samsonov Andrey. All Rights Reserved.

#pragma once

#include "Evospace/SerializableJson.h"

#include <Dom/JsonObject.h>
#include <Templates/SharedPointer.h>

#include "ItemData.generated.h"

class UConstructionBlueprint;
class UStaticIndexedItemInstancedStaticMeshComponent;
class UStaticItem;

// FItemData(const UStaticItem *item) has zero value

UCLASS(BlueprintType)
class UCustomItemData : public UObject, public ISerializableJson {
  GENERATED_BODY()
  public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UConstructionBlueprint *mBlueprint;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int64 mCharge;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual bool SerializeJson(TSharedPtr<FJsonObject> json) override;
};

USTRUCT(BlueprintType)
struct EVOSPACE_API FItemData {
  GENERATED_BODY()

  public:
  FItemData() = default;
  FItemData(const UStaticItem *item, int64 value, UCustomItemData *data = nullptr);

  FItemData &operator=(const FItemData &val);

  //  .
  bool Swap(FItemData &other);

  bool IsNull() const;

  void Clear();

  void SetItem(const UStaticItem *item);

  FORCEINLINE void SetValue(int64 value);

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int64 mValue = 0;

  bool operator==(const FItemData &other) const;
  bool operator!=(const FItemData &other) const;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
  const UStaticItem *mItem = nullptr;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
  UCustomItemData *mData = nullptr;
};

USTRUCT(BlueprintType)
struct FRecipeItemData {
  GENERATED_BODY()

  public:
  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  int64 mCapacity = 0;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  int32 mProbability = 100;

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  bool Bonus = false;

  bool DeserializeJson(TSharedPtr<FJsonObject> json);
};
