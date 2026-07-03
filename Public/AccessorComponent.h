// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "../CoordinateSystem.h"
#include "Qr/SerializableJson.h"
#include "CoreMinimal.h"
#include "Public/BlockLogic.h"
#include "Public/BlockRef.h"
#include "Qr/Prototype.h"

#include "AccessorComponent.generated.h"

//              .
//              .

class UStaticCover;
class UBlockLogic;

UCLASS()
class UAccessor : public UInstance {
  using Self = UAccessor;
  GENERATED_BODY()
  EVO_CODEGEN_ACCESSOR(Accessor)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<Self, UInstance>("Accessor") //@class Accessor : Instance
      .addProperty("side", &Self::Side) //@field Vec3i
      .addProperty("pos", &Self::Pos) //@field Vec3i
      .addProperty("owner", &Self::Owner, false) //@field BlockLogic
      .addProperty("cover", &Self::Cover) //@field StaticCover
      .endClass();
  }

  public:
  UAccessor();

  void SetSidePos(const Vec3i &side, const Vec3i &pos);

  UPROPERTY(VisibleAnywhere)
  UStaticCover *Cover = nullptr;

  const Vec3i &GetPos() const;
  const Vec3i &GetSide() const;

  UAccessor *GetOutsideAccessor(UClass *type);

  const UAccessor *GetOutsideAccessor(UClass *type) const;

  UAccessor *GetOutsideAccessor() { return GetOutsideAccessor(GetClass()); }

  const UAccessor *GetOutsideAccessor() const { return GetOutsideAccessor(GetClass()); }

  UAccessor *GetOutsideNeighborSameTypeCached();

  // TBlockRef contract: alive as long as the owner block is in the world.
  bool IsInWorld() const { return Owner && Owner->IsInWorld(); }

  template <class T>
  T *GetOutsideAccessor() { return static_cast<T *>(GetOutsideAccessor(T::StaticClass())); }

  template <class T>
  const T *GetOutsideAccessor() const {
    return static_cast<const T *>(GetOutsideAccessor(T::StaticClass()));
  }

  UBlockLogic *GetOwner() const;

  void Release();

  // No code
  virtual void TickComponent();

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
  UBlockLogic *Owner = nullptr;

  protected:
  UPROPERTY(VisibleAnywhere)
  FQrVector3i Pos;

  UPROPERTY(VisibleAnywhere)
  FQrVector3i Side;

  private:
  // Cached neighbor accessor of the same class as this accessor.
  // Validated at use (weak + owner-in-world); recomputed by position on miss.
  TBlockRef<UAccessor> CachedOutsideSameType;
};
