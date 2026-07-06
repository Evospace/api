#pragma once
#include "Qr/Vector.h"
#include "BlockLogicStore.generated.h"

class UBlockLogic;

USTRUCT(BlueprintType)
struct FBlockLogicStore {
  GENERATED_BODY()

  private:
  UPROPERTY(BlueprintType, VisibleAnywhere)
  TArray<UBlockLogic *> Blocks;

  TMap<FQrVector3i, int32> IndexMap;

  public:
  void Reset() {
    Blocks.Empty();
    IndexMap.Empty();
  }

  bool Contains(const FQrVector3i &Pos) const { return IndexMap.Contains(Pos); }

  UBlockLogic *Find(const FQrVector3i &Pos) const {
    if (const int32 *Index = IndexMap.Find(Pos)) {
      return Blocks.IsValidIndex(*Index) ? Blocks[*Index] : nullptr;
    }
    return nullptr;
  }

  UBlockLogic *Add(const FQrVector3i &Pos, UBlockLogic *Logic) {
    if (!Logic || IndexMap.Contains(Pos)) {
      return nullptr;
    }

    int32 Index = Blocks.Num();
    Blocks.Add(Logic);
    IndexMap.Add(Pos, Index);
    return Logic;
  }

  bool Remove(const FQrVector3i &Pos) {
    int32 *IndexPtr = IndexMap.Find(Pos);
    if (!IndexPtr)
      return false;

    int32 IndexToRemove = *IndexPtr;
    int32 LastIndex = Blocks.Num() - 1;

    if (IndexToRemove != LastIndex) {
      UBlockLogic *LastBlock = Blocks[LastIndex];
      Blocks[IndexToRemove] = LastBlock;

      FQrVector3i LastPos;
      for (const auto &Pair : IndexMap) {
        if (Pair.Value == LastIndex) {
          LastPos = Pair.Key;
          break;
        }
      }

      IndexMap[LastPos] = IndexToRemove;
    }

    Blocks.RemoveAt(LastIndex);
    IndexMap.Remove(Pos);

    return true;
  }

  FORCEINLINE void ForEachForward(TFunctionRef<void(UBlockLogic *)> Func) const {
    for (UBlockLogic *Logic : Blocks) {
      Func(Logic);
    }
  }

  int32 Num() const { return Blocks.Num(); }
};