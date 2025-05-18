#pragma once

#include "CoreTypes.h"
#include "Misc/AssertionMacros.h"
#include "Containers/Array.h"
#include "Math/UnrealMathUtility.h"

/**
 * Template for circular buffers.
 *
 * The size of the buffer is rounded up to the next power of two in order speed up indexing
 * operations using a simple bit mask instead of the commonly used modulus operator that may
 * be slow on some platforms.
 */
template <typename InElementType>
class EvoRingBuffer {
  public:
  using ElementType = InElementType;

  /**
   * Creates and initializes a new instance of the TCircularBuffer class.
   *
   * @param Capacity The number of elements that the buffer can store (will be rounded up to the next power of 2).
   */
  explicit EvoRingBuffer(uint32 Capacity) {
    checkSlow(Capacity > 0);
    checkSlow(Capacity <= 0xffffffffU);

    Elements.AddZeroed(FMath::RoundUpToPowerOfTwo(Capacity));
    IndexMask = Elements.Num() - 1;
  }

  /**
   * Creates and initializes a new instance of the TCircularBuffer class.
   *
   * @param Capacity The number of elements that the buffer can store (will be rounded up to the next power of 2).
   * @param InitialValue The initial value for the buffer's elements.
   */
  EvoRingBuffer(uint32 Capacity, const ElementType &InitialValue) {
    checkSlow(Capacity <= 0xffffffffU);

    Elements.Init(InitialValue, FMath::RoundUpToPowerOfTwo(Capacity));
    IndexMask = Elements.Num() - 1;
  }

  const TArray<ElementType> &GetData() const {
    return Elements;
  }

  public:
  /**
   * Returns the mutable element at the specified index.
   *
   * @param Index The index of the element to return.
   */
  FORCEINLINE ElementType &operator[](uint32 Index) {
    return Elements[Index & IndexMask];
  }

  /**
   * Returns the immutable element at the specified index.
   *
   * @param Index The index of the element to return.
   */
  FORCEINLINE const ElementType &operator[](uint32 Index) const {
    return Elements[Index & IndexMask];
  }

  FORCEINLINE void Set(uint32 Index, InElementType value) {
    Elements[Index & IndexMask] = value;
  }

  FORCEINLINE double Mean() const {
    double sum = 0;
    for (auto e : Elements) {
      sum += e;
    }

    return sum / static_cast<double>(Elements.Num());
  }

  public:
  /**
   * Returns the number of elements that the buffer can hold.
   *
   * @return Buffer capacity.
   */
  FORCEINLINE uint32 Capacity() const {
    return Elements.Num();
  }

  FORCEINLINE void Push(InElementType value) {
    Set(Tail, value);
    Tail = GetNextIndex(Tail);
  }

  FORCEINLINE ElementType GetLast() const {
    return Elements[Tail];
  }

  /**
   * Calculates the index that follows the given index.
   *
   * @param CurrentIndex The current index.
   * @return The next index.
   */
  FORCEINLINE uint32 GetNextIndex(uint32 CurrentIndex) const {
    return ((CurrentIndex + 1) & IndexMask);
  }

  /**
   * Calculates the index previous to the given index.
   *
   * @param CurrentIndex The current index.
   * @return The previous index.
   */
  FORCEINLINE uint32 GetPreviousIndex(uint32 CurrentIndex) const {
    return ((CurrentIndex - 1) & IndexMask);
  }

  private:
  /** Holds the mask for indexing the buffer's elements. */
  uint32 IndexMask;

  /** Holds the buffer's elements. */
  TArray<ElementType> Elements;

  uint32 Tail = 0;
};
