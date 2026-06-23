#pragma once
#include "FastNoiseSIMD.h"
#include "Evospace/Misc/NoiseArray.h"
#include "Qr/Prototype.h"
#include <string>
#include <unordered_map>
#include "NoiseGenerator.generated.h"

UCLASS()
/**
 * @brief Wrapper class for FastNoise that is exposed to Lua.
 */
class UNoiseGenerator : public UPrototype {
  GENERATED_BODY()
  public:
  /**
     * @brief Constructor for FastNoiseWrapper.
     * Initializes with Simplex noise by default.
     */
  UNoiseGenerator() {
    noise = std::unique_ptr<FastNoiseSIMD>(FastNoiseSIMD::NewFastNoiseSIMD());
    noise->SetNoiseType(FastNoiseSIMD::SimplexFractal);
    noise->SetFrequency(0.005f);
    noise->SetFractalOctaves(3);
  }

  /**
     * @brief Sets the seed for noise generation.
     * @param seed The seed value.
     */
  void set_seed(int seed) { noise->SetSeed(seed + seed_offset); }

  /**
     * @brief Generates 3D noise based on the current settings.
     * @param x The X coordinate.
     * @param y The Y coordinate.
     * @param z The Z coordinate.
     * @return The noise value.
     */
  float get_noise(float x, float y, float z) {
    FNoiseArray na;
    noise->FillNoiseSet(na.get(), x, y, z, 1, 1, 1);
    return na[0];
  }

  /**
     * @brief Sets the noise type ("Simplex", "SimplexFractal", "Perlin", "SimplexFractal", "PerlinFractal",
     * "Cellular", "Value", "ValueFractal", "Cubic", "CubicFractal", "White").
     * @param noise_type The type of noise to generate.
     */
  void set_noise_type(const std::string &noise_type) {
    static const std::unordered_map<std::string, TFunction<void(UNoiseGenerator *)>> noise_map = {
      { "Simplex", +[](UNoiseGenerator *self) { self->noise->SetNoiseType(FastNoiseSIMD::Simplex); } },
      { "SimplexFractal",
        +[](UNoiseGenerator *self) { self->noise->SetNoiseType(FastNoiseSIMD::SimplexFractal); } },
      { "Perlin", +[](UNoiseGenerator *self) { self->noise->SetNoiseType(FastNoiseSIMD::Perlin); } },
      { "PerlinFractal", +[](UNoiseGenerator *self) { self->noise->SetNoiseType(FastNoiseSIMD::PerlinFractal); } },
      { "Cellular", +[](UNoiseGenerator *self) { self->noise->SetNoiseType(FastNoiseSIMD::Cellular); } },
      { "Value", +[](UNoiseGenerator *self) { self->noise->SetNoiseType(FastNoiseSIMD::Value); } },
      { "ValueFractal", +[](UNoiseGenerator *self) { self->noise->SetNoiseType(FastNoiseSIMD::ValueFractal); } },
      { "Cubic", +[](UNoiseGenerator *self) { self->noise->SetNoiseType(FastNoiseSIMD::Cubic); } },
      { "CubicFractal", +[](UNoiseGenerator *self) { self->noise->SetNoiseType(FastNoiseSIMD::CubicFractal); } },
      { "White", +[](UNoiseGenerator *self) { self->noise->SetNoiseType(FastNoiseSIMD::WhiteNoise); } },
    };

    auto it = noise_map.find(noise_type);
    if (it != noise_map.end()) {
      it->second(this);
    }
  }

  /**
     * @brief Sets the frequency for noise generation.
     * @param frequency The frequency value.
     */
  void set_frequency(float frequency) const { noise->SetFrequency(frequency); }

  /**
     * @brief Sets the fractal octaves for noise generation.
     * @param octaves The number of octaves.
     */
  void set_fractal_octaves(int octaves) const { noise->SetFractalOctaves(octaves); }

  /**
     * @brief Sets the fractal gain for noise generation.
     * @param gain The gain value.
     */
  void set_fractal_gain(float gain) { noise->SetFractalGain(gain); }

  /**
     * @brief Sets the fractal lacunarity for noise generation.
     * @param lacunarity The lacunarity value.
     */
  void set_fractal_lacunarity(float lacunarity) { noise->SetFractalLacunarity(lacunarity); }

  /**
     * @brief Sets the fractal type for noise generation.
     * @param fractal_type The fractal type (e.g., "FBM", "RigidMulti", "Billow").
     */
  void set_fractal_type(const std::string &fractal_type) const {
    if (fractal_type == "Fbm") {
      noise->SetFractalType(FastNoiseSIMD::FBM);
    } else if (fractal_type == "Ridged") {
      noise->SetFractalType(FastNoiseSIMD::RigidMulti);
    } else if (fractal_type == "Billow") {
      noise->SetFractalType(FastNoiseSIMD::Billow);
    }
  }

  void set_cellular_return_type(const std::string &return_type) {
    static const std::unordered_map<std::string, FastNoiseSIMD::CellularReturnType> return_map = {
      { "CellValue", FastNoiseSIMD::CellValue },
      { "Distance", FastNoiseSIMD::Distance },
      { "Distance2", FastNoiseSIMD::Distance2 },
      { "Distance2Add", FastNoiseSIMD::Distance2Add },
      { "Distance2Sub", FastNoiseSIMD::Distance2Sub },
      { "Distance2Mul", FastNoiseSIMD::Distance2Mul },
      { "Distance2Div", FastNoiseSIMD::Distance2Div },
      { "NoiseLookup", FastNoiseSIMD::NoiseLookup },
      { "Distance2Cave", FastNoiseSIMD::Distance2Cave },
    };
    const auto it = return_map.find(return_type);
    if (it != return_map.end()) {
      noise->SetCellularReturnType(it->second);
    }
  }

  void set_cellular_distance_function(const std::string &distance_function) {
    static const std::unordered_map<std::string, FastNoiseSIMD::CellularDistanceFunction> distance_map = {
      { "Euclidean", FastNoiseSIMD::Euclidean },
      { "Manhattan", FastNoiseSIMD::Manhattan },
      { "Natural", FastNoiseSIMD::Natural },
    };
    const auto it = distance_map.find(distance_function);
    if (it != distance_map.end()) {
      noise->SetCellularDistanceFunction(it->second);
    }
  }

  std::unique_ptr<FastNoiseSIMD> noise;
  float min = 2;
  float max = 10;
  // Added to the world seed before being applied to the noise so noises in a
  // stack decorrelate from one another. Authored via JSON ("SeedOffset").
  int32 seed_offset = 0;

  // JSON-driven configuration so height noises can be authored as content
  // (parallel to the Lua setters above). Reads NoiseType, Frequency,
  // FractalOctaves, FractalGain, FractalLacunarity, FractalType, Min, Max,
  // SeedOffset.
  virtual bool DeserializeJson(TSharedPtr<FJsonObject> json) override;

  public:
  PROTOTYPE_CODEGEN(NoiseGenerator, NoiseGenerator)
  virtual void lua_reg(lua_State *L) const override {
    luabridge::getGlobalNamespace(L)
      .deriveClass<UNoiseGenerator, UPrototype>("NoiseGenerator") //@class NoiseGenerator : Prototype
      // direct:
      //---Set the seed for noise generation
      //---@param seed integer Seed value
      // function NoiseGenerator:set_seed(seed) end
      .addFunction("set_seed", &UNoiseGenerator::set_seed)
      // direct:
      //---Get the noise value at the given position
      //---@param x number X coordinate
      //---@param y number Y coordinate
      //---@param z number Z coordinate
      // function NoiseGenerator:get_noise(x, y, z) end
      .addFunction("get_noise", &UNoiseGenerator::get_noise)
      // direct:
      //---Set the noise type
      //---@param noise_type string Noise type
      // function NoiseGenerator:set_noise_type(noise_type) end
      .addFunction("set_noise_type", &UNoiseGenerator::set_noise_type)
      // direct:
      //---Set the frequency for noise generation
      //---@param frequency number Frequency value
      // function NoiseGenerator:set_frequency(frequency) end
      .addFunction("set_frequency", &UNoiseGenerator::set_frequency)
      // direct:
      //---Set the fractal octaves for noise generation
      //---@param octaves integer Octaves value
      // function NoiseGenerator:set_fractal_octaves(octaves) end
      .addFunction("set_fractal_octaves", &UNoiseGenerator::set_fractal_octaves)
      // direct:
      //---Set the fractal gain for noise generation
      //---@param gain number Gain value
      // function NoiseGenerator:set_fractal_gain(gain) end
      .addFunction("set_fractal_gain", &UNoiseGenerator::set_fractal_gain)
      // direct:
      //---Set the fractal lacunarity for noise generation
      //---@param lacunarity number Lacunarity value
      // function NoiseGenerator:set_fractal_lacunarity(lacunarity) end
      .addFunction("set_fractal_lacunarity", &UNoiseGenerator::set_fractal_lacunarity)
      // direct:
      //---Set the fractal type for noise generation
      //---@param fractal_type string Fractal type
      // function NoiseGenerator:set_fractal_type(fractal_type) end
      .addFunction("set_fractal_type", &UNoiseGenerator::set_fractal_type)
      .addProperty("min", &UNoiseGenerator::min) //@field number
      .addProperty("max", &UNoiseGenerator::max) //@field number
      .endClass();
  }
  virtual UClass *GetSuperProto() const override { return StaticClass(); }
};