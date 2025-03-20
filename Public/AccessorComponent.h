// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "../CoordinateSystem.h"
#include "../SerializableJson.h"
#include "CoreMinimal.h"
#include "Public/BlockLogic.h"
#include "Public/Prototype.h"

#include "AccessorComponent.generated.h"

//              .
//              .

class UStaticCover;
class UBlockLogic;

UCLASS()
class EVOSPACE_API UAccessor : public UInstance {
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
  UStaticCover *Cover;

  const Vec3i &GetPos() const;
  const Vec3i &GetSide() const;

  UAccessor *GetOutsideAccessor(UClass *type);

  const UAccessor *GetOutsideAccessor(UClass *type) const;

  UAccessor *GetOutsideAccessor() { return GetOutsideAccessor(GetClass()); }

  const UAccessor *GetOutsideAccessor() const {
    return GetOutsideAccessor(GetClass());
  }

  template <class T>
  T *GetOutsideAccessor() {
    return static_cast<T *>(GetOutsideAccessor(T::StaticClass()));
  }

  template <class T>
  const T *GetOutsideAccessor() const {
    return static_cast<const T *>(GetOutsideAccessor(T::StaticClass()));
  }

  UBlockLogic *GetOwner() const;

  void Release();

  // No code
  virtual void TickComponent();

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
  UBlockLogic *Owner;

  protected:
  UPROPERTY(VisibleAnywhere)
  FVector3i Pos;

  UPROPERTY(VisibleAnywhere)
  FVector3i Side;
};
