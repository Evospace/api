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

  TMap<FVector3i, int32> IndexMap;

  bool Flip = false;

  public:
  void Reset() {
    Blocks.Empty();
    IndexMap.Empty();
  }

  bool Contains(const FVector3i &Pos) const { return IndexMap.Contains(Pos); }

  UBlockLogic *Find(const FVector3i &Pos) const {
    if (const int32 *Index = IndexMap.Find(Pos)) {
      return Blocks.IsValidIndex(*Index) ? Blocks[*Index] : nullptr;
    }
    return nullptr;
  }

  UBlockLogic *Add(const FVector3i &Pos, UBlockLogic *Logic) {
    if (!Logic || IndexMap.Contains(Pos)) {
      return nullptr;
    }

    int32 Index = Blocks.Num();
    Blocks.Add(Logic);
    IndexMap.Add(Pos, Index);
    return Logic;
  }

  bool Remove(const FVector3i &Pos) {
    int32 *IndexPtr = IndexMap.Find(Pos);
    if (!IndexPtr)
      return false;

    int32 IndexToRemove = *IndexPtr;
    int32 LastIndex = Blocks.Num() - 1;

    if (IndexToRemove != LastIndex) {
      UBlockLogic *LastBlock = Blocks[LastIndex];
      Blocks[IndexToRemove] = LastBlock;

      FVector3i LastPos;
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

  void ForEachFlipFlop(TFunctionRef<void(UBlockLogic *)> Func) {
    if (Flip) {
      ForEachForward(Func);
    } else {
      ForEachReverse(Func);
    }
    Flip = !Flip;
  }

  void ForEachForward(TFunctionRef<void(UBlockLogic *)> Func) const {
    for (UBlockLogic *Logic : Blocks) {
      Func(Logic);
    }
  }

  void ForEachReverse(TFunctionRef<void(UBlockLogic *)> Func) const {
    for (int32 i = Blocks.Num() - 1; i >= 0; --i) {
      Func(Blocks[i]);
    }
  }

  int32 Num() const { return Blocks.Num(); }
};