#pragma once

#include "Public/WorldGenerator.h"
#include "CoreMinimal.h"
#include "FastNoiseSIMD.h"
#include "PropList.h"
#include "array"
#include "functional"
#include "map"
#include "memory"
#include "vector"

#include "MiscGeneratorClasses.generated.h"
#define SetMacro()                                                            \
  start_point.X * 2 + 4, start_point.Y * 2 + 4, 1, gFlatSectorSize.X * 2 + 4, \
    gFlatSectorSize.Y * 2 + 4, 1

using BlockInterval = std::pair<unsigned short, unsigned short>;

class UStaticBlock;
class UStaticProp;
class UStaticPropList;
class ULayeringGenerator;
class UBiome;

struct LayeringNode {
  LayeringNode(UStaticBlock *_block)
    : block(_block) {}

  LayeringNode() {}

  UStaticBlock *block = nullptr;
};

struct Layering {
  std::map<unsigned short, LayeringNode, std::greater<unsigned short>> layers;

  UStaticBlock *at(int depth) {
    const auto &l_node = layers.lower_bound(depth);

    if (ensure(l_node != layers.end())) {
      return std::get<1>(*l_node).block;
    }

    return nullptr;
  }
};

enum class StructureSize {
  s32x32,
  s64x64,
  s128x128,
};

constexpr auto gFlatForBiCubicSize = gFlatSectorSize + 4;
constexpr auto gFlatForBiCubicCapacity = gFlatForBiCubicSize.Capacity();

enum {
  GeneratorOctaveCount = 4
};
enum {
  SmoothKernelRadius = 20
};
enum {
  DensSmoothKernelRadius = 3
};
constexpr Vec3i PrecomputedBiomesize =
  gTallSectorSize + Vec3i{ SmoothKernelRadius * 2, SmoothKernelRadius * 2, SmoothKernelRadius * 2 };
constexpr Vec3i PrecomputedDensSize =
  gTallSectorSize + Vec3i{ DensSmoothKernelRadius * 2,
                           DensSmoothKernelRadius * 2,
                           DensSmoothKernelRadius * 2 };

template <typename Ty_>
using SmoothBuffer = std::array<std::array<Ty_, PrecomputedBiomesize.Y>, PrecomputedBiomesize.X>;

template <typename Ty_>
using SectorBuffer = std::array<std::array<Ty_, gFlatSectorSize.Y>, gFlatSectorSize.X>;

template <typename Ty_>
using SectorBufferHeap = std::vector<Ty_>;

template <typename Ty_>
using Smooth3DBuffer = std::vector<Ty_>;

template <typename Ty_>
using Sector3DBuffer = std::vector<Ty_>;

constexpr Vec2i just_block{ gFlatSectorSize };
constexpr Vec2i just_block_smooth{ gFlatSectorSize + SmoothKernelRadius * 2 };
constexpr Vec2i sub_block{ gFlatSectorSize * 2.f };
constexpr Vec2i baked_size{ gFlatSectorSize * 2.f + 4.f }; // sub block + bicubic
constexpr Vec2i baked_smooth_size{ (gFlatSectorSize + SmoothKernelRadius) * 2.f }; // sub block + smooth

UCLASS()
class ULayeringGenerator : public UPrototype {
  GENERATED_BODY()
  PROTOTYPE_CODEGEN(LayeringGenerator, LayeringGenerator)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<ULayeringGenerator, UPrototype>("LayeringGenerator") //@class LayeringGenerator : Prototype
      .endClass();
  }
  virtual UClass *GetSuperProto() const override {
    return StaticClass();
  }

  public:
  virtual Layering GetLayering(const Vec2i &pos) const;
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual void SetSeed(int32 seed);

  UPROPERTY()
  UBiome *mBiome;

  protected:
  TArray<UStaticBlock *> mBlocks;
};

UCLASS()
class USimpleLayeringGenerator : public ULayeringGenerator {
  GENERATED_BODY()
  PROTOTYPE_CODEGEN(SimpleLayeringGenerator, LayeringGenerator)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<USimpleLayeringGenerator, ULayeringGenerator>("SimpleLayeringGenerator") //@class SimpleLayeringGenerator : LayeringGenerator
      .endClass();
  }
  virtual UClass *GetSuperProto() const override {
    return ULayeringGenerator::StaticClass();
  }

  public:
  virtual Layering GetLayering(const Vec2i &pos) const override;
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  protected:
  TArray<UStaticBlock *> mLayerBlock;
  TArray<int32> mLayerStart;
};

UCLASS()
class UChancedLayeringGenerator : public USimpleLayeringGenerator {
  GENERATED_BODY()
  PROTOTYPE_CODEGEN(ChancedLayeringGenerator, LayeringGenerator)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UChancedLayeringGenerator, USimpleLayeringGenerator>("ChancedLayeringGenerator") //@class ChancedLayeringGenerator : SimpleLayeringGenerator
      .endClass();
  }
  virtual UClass *GetSuperProto() const override {
    return ULayeringGenerator::StaticClass();
  }

  public:
  virtual Layering GetLayering(const Vec2i &pos) const override;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  protected:
  TArray<UStaticBlock *> mLayerChanceBlock;
  TArray<float> mChances;
};

UCLASS()
class UPropsGenerator : public UPrototype {
  GENERATED_BODY()
  PROTOTYPE_CODEGEN(PropsGenerator, PropsGenerator)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UPropsGenerator, UPrototype>("SimpleLayeringGenerator") //@class PropsGenerator : Prototype
      .endClass();
  }
  virtual UClass *GetSuperProto() const override {
    return StaticClass();
  }

  public:
  virtual const UStaticProp *GetSurfaceAttach(FRandomStream &rnd, const Vec2i &start_point) const;
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  UPROPERTY()
  UBiome *mBiome = nullptr;

  virtual void SetSeed(int32 seed);

  UPROPERTY()
  UStaticPropList *proplist = nullptr;
};

UCLASS()
class UBiome : public UPrototype {
  GENERATED_BODY()
  PROTOTYPE_CODEGEN(Biome, Biome)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UBiome, UPrototype>("Biome") //@class Biome : Prototype
      .endClass();
  }
  virtual UClass *GetSuperProto() const override {
    return StaticClass();
  }

  public:
  virtual Layering GetLayering(const Vec2i &pos) const;
  virtual float GetHeight(const FVector2D &pos) const;
  virtual float GetGrad(const FVector2D &pos) const;
  virtual const UStaticProp *GetSurfaceAttach(FRandomStream &rnd, const Vec2i &pos) const;
  virtual float GetWeight(const FVector2D &pos) const;
  virtual IndexType GetBiome(const Vec2i &pos) const;
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual void SetSeed(int32 seed);

  UPROPERTY()
  ULayeringGenerator *layering = nullptr;

  UPROPERTY()
  UPropsGenerator *props = nullptr;
};

UCLASS()
class UBiomeFamily : public UBiome {
  GENERATED_BODY()
  PROTOTYPE_CODEGEN(BiomeFamily, Biome)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UBiomeFamily, UBiome>("BiomeFamily")
      .addProperty("sub_biomes", &UBiomeFamily::mSubBiomes)
      .addProperty("sub_frequency", &UBiomeFamily::mSubFrequency)
      .endClass();
  }
  virtual UClass *GetSuperProto() const override {
    return UBiome::StaticClass();
  }

  public:
  UBiomeFamily();

  // Only baked
  virtual Layering GetLayering(const Vec2i &pos) const override;

  virtual float GetHeight(const FVector2D &pos) const override;

  virtual float GetGrad(const FVector2D &pos) const override;

  // Only baked
  virtual const UStaticProp *GetSurfaceAttach(FRandomStream &rnd, const Vec2i &pos) const override;

  // Only baked
  virtual IndexType GetBiome(const Vec2i &pos) const override;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  virtual void SetSeed(int32 seed) override;

  std::vector<UBiome *> mSubBiomes;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  float mSubFrequency = 0.016f;

  protected:
  std::unique_ptr<FastNoiseSIMD> mBiomeNoise;
};
