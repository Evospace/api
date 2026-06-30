#pragma once

#include "Public/WorldGenerator.h"
#include "CoreMinimal.h"
#include "FastNoiseSIMD.h"
#include "PropList.h"
#include "Qr/Prototype.h"
#include "Evospace/Shared/Public/Biome.h"

#include <array>
#include <functional>
#include <map>
#include <memory>
#include <vector>

#include "MiscGeneratorClasses.generated.h"
#define SetMacro() \
  start_point.X * 2 + 4, start_point.Y * 2 + 4, 1, gFlatSectorSize.X * 2 + 4, gFlatSectorSize.Y * 2 + 4, 1

using BlockInterval = std::pair<unsigned short, unsigned short>;

class UStaticBlock;
class UStaticProp;
class UStaticPropList;
class ULayeringGenerator;
class UBiome;
class UHeightGenerator;

namespace mapgen {
// Single-pass separable Gaussian blur (finite support, truncated at `radius`)
// over a row-major [w*h] grid. Because support is exactly `radius`, evaluating
// it over a halo that extends `radius` beyond the region of interest makes
// inner-cell results independent of the halo border (clamped replicate edges
// only reach border cells) — that is what makes blended heights identical
// across adjacent sectors. O(N*(2*radius+1)) per axis. `in`/`out` are [w*h] and
// must not alias; scratch is allocated internally.
void GaussianBlur2D(const float *in, float *out, int32 w, int32 h, int32 radius);

// Weighted per-biome height blend (see biome_plan.md target pipeline):
//   for each distinct biome b: Wb = Blur(mask_b, radius);
//   out = Σ_b Wb·Hb / Σ_b Wb
// biomeId/out are [w*h]; `heights` holds one [w*h] field per entry of
// `distinctBiomes` (parallel arrays). Preserves interior detail, blends seams.
void BlendBiomeHeights(const int32 *biomeId, int32 w, int32 h, int32 radius,
                       const TArray<int32> &distinctBiomes,
                       const TArray<const float *> &heights,
                       float *out);
} // namespace mapgen

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
  gTallSectorSize + Vec3i{ DensSmoothKernelRadius * 2, DensSmoothKernelRadius * 2, DensSmoothKernelRadius * 2 };

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
  virtual UClass *GetSuperProto() const override { return StaticClass(); }

  public:
  virtual FLayering GetLayering(const Vec2i &pos) const;
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual void SetSeed(int32 seed);

  protected:
  TArray<UStaticBlock *> mBlocks;
};

UCLASS()
class USimpleLayeringGenerator : public ULayeringGenerator {
  GENERATED_BODY()
  PROTOTYPE_CODEGEN(SimpleLayeringGenerator, LayeringGenerator)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<USimpleLayeringGenerator, ULayeringGenerator>(
        "SimpleLayeringGenerator") //@class SimpleLayeringGenerator : LayeringGenerator
      .endClass();
  }
  virtual UClass *GetSuperProto() const override { return ULayeringGenerator::StaticClass(); }

  public:
  virtual FLayering GetLayering(const Vec2i &pos) const override;
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
      .deriveClass<UChancedLayeringGenerator, USimpleLayeringGenerator>(
        "ChancedLayeringGenerator") //@class ChancedLayeringGenerator : SimpleLayeringGenerator
      .endClass();
  }
  virtual UClass *GetSuperProto() const override { return ULayeringGenerator::StaticClass(); }

  public:
  virtual FLayering GetLayering(const Vec2i &pos) const override;

  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  protected:
  TArray<UStaticBlock *> mLayerChanceBlock;
  TArray<float> mChances;
};

// Selects between two block stacks by the local surface detail height: where a
// referenced HeightGenerator's detail at (x,y) drops below `Below`, the "low"
// stack is used (e.g. clay flats in basins), otherwise the inherited base stack
// (Blocks/Starts). Lets a single leaf biome read as grass on the swells and a
// different material only in the carved lowlands instead of one material edge
// to edge. JSON: "LowBlocks"/"LowStarts" (the low stack), "Height" (a
// HeightGenerator name, normally the same basin field the biome height uses)
// and "Below" (threshold). Seam-safe: the height field is world-position based.
UCLASS()
class ULowlandLayeringGenerator : public USimpleLayeringGenerator {
  GENERATED_BODY()
  PROTOTYPE_CODEGEN(LowlandLayeringGenerator, LayeringGenerator)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<ULowlandLayeringGenerator, USimpleLayeringGenerator>(
        "LowlandLayeringGenerator") //@class LowlandLayeringGenerator : SimpleLayeringGenerator
      .endClass();
  }
  virtual UClass *GetSuperProto() const override { return ULayeringGenerator::StaticClass(); }

  public:
  virtual FLayering GetLayering(const Vec2i &pos) const override;
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;
  virtual void SetSeed(int32 seed) override;

  protected:
  TArray<UStaticBlock *> mLowBlock;
  TArray<int32> mLowStart;
  UHeightGenerator *mHeight = nullptr;
  float mBelow = 0.f;
};