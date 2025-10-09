--- @type boolean|nil
LuaLogFlag = false

--- Instancable object
---
--- @class Instance : Object
Instance = {}

--- Integer vector 3
---
--- @class Vec3i
--- @field zero Vec3i = (0, 0, 0)
--- @field one Vec3i = (1, 1, 1)
--- @field left Vec3i = (0, 1, 0)
--- @field right Vec3i = (0, -1, 0)
--- @field up Vec3i = (0, 0, 1)
--- @field down Vec3i = (0, 0, -1)
--- @field front Vec3i = (1, 0, 0)
--- @field back Vec3i = (-1, 0, 0)
--- @field x integer
--- @field y integer
--- @field z integer
Vec3i = {}

--- Float vector 3
---
--- @class Vec3
--- @field zero Vec3 = (0, 0, 0)
--- @field one Vec3 = (1, 1, 1)
--- @field left Vec3 = (0, 1, 0)
--- @field right Vec3 = (0, -1, 0)
--- @field up Vec3 = (0, 0, 1)
--- @field down Vec3 = (0, 0, -1)
--- @field front Vec3 = (1, 0, 0)
--- @field back Vec3 = (-1, 0, 0)
--- @field x number
--- @field y number
--- @field z number
Vec3 = {}

--- @param x integer 
--- @param y integer 
--- @param z integer 
--- @return Vec3i
function Vec3i.new(x, y, z) end

--- Integer vector 2
---
--- @class Vec2i
--- @field zero Vec2i
--- @field one Vec2i
--- @field x integer
--- @field y integer
Vec2i = {}

--- @param x integer 
--- @param y integer 
--- @return Vec2i
function Vec2i.new(x, y) end

---
---
--- @class Class
Class = {}

---
---
--- @class Texture
Texture = {}

---
---
--- @class Material
Material = {}

--- @param path string Path to the object
--- @return Material
function Material.load(path) end

--- Global object
---
--- @class Object
Object = {}

--- Global object
---
--- @class DB
db = {}

--- Global object
---
--- @class RegionMap
regions = {}

--- Non localized data entry
---
--- @class LocData
LocData = {}

--- Set non localized data value by key
--- @param key string
--- @param table string
function LocData.set(key, table) end

--- Localization key table object
---
--- @class Kto
Kto = {}

--- Create new Kto object
--- @param key string
--- @param table string
--- @return Loc
function Kto.new(key, table) end

--- Resolve Kto to localized string
--- @return string
function Kto:get() end

--- Localization data entry
---
--- @class Loc
Loc = {}

--- Create new Loc object
--- @param key string
--- @param table string
--- @return Loc
function Loc.new(key, table) end

--- Resolve key value to localized string
--- @param key string
--- @param table string
--- @return string
function Loc.get(key, table) end

--- Resolve Loc object to localized string
--- @return string
function Loc:get() end

--- Get number as string for GUI (3000 -> 3.0k)
--- @param value number
--- @return string
function Loc.gui_number(value) end

--- Prints error to log but do not breaks execution
--- @param message string
function print_err(message) end

--- Prints warning to log
--- @param message string
function print_warn(message) end

--- @class Game
game = {}


--- Engine sound class
---
--- @class SoundClass
--- @field volume number
SoundClass = {}

--- Engine static mesh
---
--- @class StaticMesh
--- @field name string
StaticMesh = {}

--- @param path string path to the object
--- @return StaticMesh
function StaticMesh.load(path) end

--- @param path string path to the object
--- @return SoundClass
function SoundClass.load(path) end

---
---
--- @class Dimension
dim = {}

---@meta
-- Lua bindings for coordinate system conversion (via LuaBridge).
-- Provides utility functions for converting between block, sector, and world coordinates.

---@class cs
cs = {}

---Convert a block position to a sector position.
---@param bpos Vec3i # Block position (integer vector).
---@return Vec3i     # Sector position.
function cs.bp2sp(bpos) end

---Convert a world position to a block position.
---@param world FVector # World position (floating-point vector).
---@return Vec3i        # Block position (integer vector).
function cs.w2bp(world) end

---Convert a block position to a world position.
---@param bpos Vec3i # Block position (integer vector).
---@return FVector   # World position (floating-point vector).
function cs.bp2w(bpos) end

---Convert a world position to a sector position.
---@param world FVector # World position (floating-point vector).
---@return Vec3i        # Sector position.
function cs.w2sp(world) end

---Global sector size.
---@type Vec3i
cs.sector_size = Vec3i.new(16, 16, 16)


-- end of common --

--- 
--- 
--- @class AbstractCrafter : BlockLogic
--- @field recipes RecipeDictionary undocumented
--- @field load_independent boolean undocumented
--- @field stable_supply boolean undocumented
--- @field input_gathered boolean undocumented
--- @field switch_on boolean undocumented
--- @field ticks_passed integer undocumented
--- @field real_ticks_passed integer undocumented
--- @field total_production integer undocumented
--- @field speed integer undocumented
--- @field energy_input_inventory ResourceInventory undocumented
--- @field energy_output_inventory ResourceInventory undocumented
--- @field crafter_input_container InventoryContainer undocumented
--- @field crafter_output_container InventoryContainer undocumented
AbstractCrafter = {}

--- Creates a new AbstractCrafter instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return AbstractCrafter
function AbstractCrafter.new(parent, name) end

--- Creates a new AbstractCrafter instance
--- @return AbstractCrafter
function AbstractCrafter.new_simple() end

--- Return AbstractCrafter class object
--- @return Class
function AbstractCrafter.get_class() end

--- Trying to cast Object into AbstractCrafter
--- @param object Object to cast
--- @return AbstractCrafter
function AbstractCrafter.cast(object) end

--- 
--- 
--- @class Accessor : Instance
--- @field side Vec3i undocumented
--- @field pos Vec3i undocumented
--- @field owner BlockLogic undocumented
--- @field cover StaticCover undocumented
Accessor = {}

--- Creates a new Accessor instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return Accessor
function Accessor.new(parent, name) end

--- Return Accessor class object
--- @return Class
function Accessor.get_class() end

--- Trying to cast Object into Accessor
--- @param object Object to cast
--- @return Accessor
function Accessor.cast(object) end

--- Achievement progress record
--- 
--- @class AchievementRecord : Prototype
--- @field delta integer undocumented
--- @field total integer undocumented
--- @field threshold integer undocumented
--- @field unlocked boolean undocumented
--- @field unlocked_time number undocumented
--- @field image Texture undocumented
--- @field steam_key string undocumented
--- @field label Loc undocumented
--- @field description Loc undocumented
AchievementRecord = {}

--- Register a new StaticAchievement static object
--- @param name string The name of the object
--- @return StaticAchievement
function StaticAchievement.reg(name) end

--- Searching for StaticAchievement in db
--- @param name string The name of the object
--- @return StaticAchievement
function StaticAchievement.find(name) end

--- Return StaticAchievement class object
--- @return Class
function StaticAchievement.get_class() end

--- Trying to cast Object into StaticAchievement
--- @param object Object to cast
--- @return StaticAchievement
function StaticAchievement.cast(object) end

--- 
--- 
--- @class AutoCrafter : SelectCrafter
AutoCrafter = {}

--- Creates a new AutoCrafter instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return AutoCrafter
function AutoCrafter.new(parent, name) end

--- Creates a new AutoCrafter instance
--- @return AutoCrafter
function AutoCrafter.new_simple() end

--- Return AutoCrafter class object
--- @return Class
function AutoCrafter.get_class() end

--- Trying to cast Object into AutoCrafter
--- @param object Object to cast
--- @return AutoCrafter
function AutoCrafter.cast(object) end

--- 
--- 
--- @class AutosizeInventory : Inventory
AutosizeInventory = {}

--- Creates a new AutosizeInventory instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return AutosizeInventory
function AutosizeInventory.new(parent, name) end

--- Creates a new AutosizeInventory instance
--- @return AutosizeInventory
function AutosizeInventory.new_simple() end

--- Return AutosizeInventory class object
--- @return Class
function AutosizeInventory.get_class() end

--- Trying to cast Object into AutosizeInventory
--- @param object Object to cast
--- @return AutosizeInventory
function AutosizeInventory.cast(object) end

--- 
--- 
--- @class BaseInventory : InventoryAccess
BaseInventory = {}

--- Creates a new BaseInventory instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return BaseInventory
function BaseInventory.new(parent, name) end

--- Creates a new BaseInventory instance
--- @return BaseInventory
function BaseInventory.new_simple() end

--- Return BaseInventory class object
--- @return Class
function BaseInventory.get_class() end

--- Trying to cast Object into BaseInventory
--- @param object Object to cast
--- @return BaseInventory
function BaseInventory.cast(object) end

--- 
--- 
--- @class Biome : Prototype
Biome = {}

--- Register a new Biome static object
--- @param name string The name of the object
--- @return Biome
function Biome.reg(name) end

--- Searching for Biome in db
--- @param name string The name of the object
--- @return Biome
function Biome.find(name) end

--- Return Biome class object
--- @return Class
function Biome.get_class() end

--- Trying to cast Object into Biome
--- @param object Object to cast
--- @return Biome
function Biome.cast(object) end

--- 
--- 
--- @class BiomeFamily : Biome
BiomeFamily = {}

--- Register a new BiomeFamily static object
--- @param name string The name of the object
--- @return BiomeFamily
function BiomeFamily.reg(name) end

--- Searching for BiomeFamily in db
--- @param name string The name of the object
--- @return BiomeFamily
function BiomeFamily.find(name) end

--- Return BiomeFamily class object
--- @return Class
function BiomeFamily.get_class() end

--- Trying to cast Object into BiomeFamily
--- @param object Object to cast
--- @return BiomeFamily
function BiomeFamily.cast(object) end

--- 
--- 
--- @class BiomeWorldGenerator : WorldGenerator
--- @field global_biome GlobalBiomeFamily undocumented
BiomeWorldGenerator = {}

--- Register a new BiomeWorldGenerator static object
--- @param name string The name of the object
--- @return BiomeWorldGenerator
function BiomeWorldGenerator.reg(name) end

--- Searching for BiomeWorldGenerator in db
--- @param name string The name of the object
--- @return BiomeWorldGenerator
function BiomeWorldGenerator.find(name) end

--- Return BiomeWorldGenerator class object
--- @return Class
function BiomeWorldGenerator.get_class() end

--- Trying to cast Object into BiomeWorldGenerator
--- @param object Object to cast
--- @return BiomeWorldGenerator
function BiomeWorldGenerator.cast(object) end

--- 
--- 
--- @class BlockLogic : Instance
--- @field static_block StaticBlock comment
BlockLogic = {}

--- Register Accessor
--- @param accessor Accessor Accessor instance to register
 function BlockLogic:reg(accessor) end

--- Creates a new BlockLogic instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return BlockLogic
function BlockLogic.new(parent, name) end

--- Creates a new BlockLogic instance
--- @return BlockLogic
function BlockLogic.new_simple() end

--- Return BlockLogic class object
--- @return Class
function BlockLogic.get_class() end

--- Trying to cast Object into BlockLogic
--- @param object Object to cast
--- @return BlockLogic
function BlockLogic.cast(object) end

--- 
--- 
--- @class ChancedLayeringGenerator : SimpleLayeringGenerator
ChancedLayeringGenerator = {}

--- Register a new LayeringGenerator static object
--- @param name string The name of the object
--- @return LayeringGenerator
function LayeringGenerator.reg(name) end

--- Searching for LayeringGenerator in db
--- @param name string The name of the object
--- @return LayeringGenerator
function LayeringGenerator.find(name) end

--- Return LayeringGenerator class object
--- @return Class
function LayeringGenerator.get_class() end

--- Trying to cast Object into LayeringGenerator
--- @param object Object to cast
--- @return LayeringGenerator
function LayeringGenerator.cast(object) end

--- Register a new SimpleLayeringGenerator static object
--- @param name string The name of the object
--- @return SimpleLayeringGenerator
function SimpleLayeringGenerator.reg(name) end

--- Searching for SimpleLayeringGenerator in db
--- @param name string The name of the object
--- @return SimpleLayeringGenerator
function SimpleLayeringGenerator.find(name) end

--- Return SimpleLayeringGenerator class object
--- @return Class
function SimpleLayeringGenerator.get_class() end

--- Trying to cast Object into SimpleLayeringGenerator
--- @param object Object to cast
--- @return SimpleLayeringGenerator
function SimpleLayeringGenerator.cast(object) end

--- Register a new ChancedLayeringGenerator static object
--- @param name string The name of the object
--- @return ChancedLayeringGenerator
function ChancedLayeringGenerator.reg(name) end

--- Searching for ChancedLayeringGenerator in db
--- @param name string The name of the object
--- @return ChancedLayeringGenerator
function ChancedLayeringGenerator.find(name) end

--- Return ChancedLayeringGenerator class object
--- @return Class
function ChancedLayeringGenerator.get_class() end

--- Trying to cast Object into ChancedLayeringGenerator
--- @param object Object to cast
--- @return ChancedLayeringGenerator
function ChancedLayeringGenerator.cast(object) end

--- 
--- 
--- @class ChestBlockLogic : StorageBlockLogic
--- @field capacity integer Chest slot count
ChestBlockLogic = {}

--- Creates a new ChestBlockLogic instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return ChestBlockLogic
function ChestBlockLogic.new(parent, name) end

--- Creates a new ChestBlockLogic instance
--- @return ChestBlockLogic
function ChestBlockLogic.new_simple() end

--- Return ChestBlockLogic class object
--- @return Class
function ChestBlockLogic.get_class() end

--- Trying to cast Object into ChestBlockLogic
--- @param object Object to cast
--- @return ChestBlockLogic
function ChestBlockLogic.cast(object) end

--- 
--- 
--- @class ComputerBlockLogic : BlockLogic
--- @field energy_inventory ResourceInventory undocumented
--- @field energy_input ResourceAccessor undocumented
ComputerBlockLogic = {}

--- Creates a new ComputerBlockLogic instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return ComputerBlockLogic
function ComputerBlockLogic.new(parent, name) end

--- Creates a new ComputerBlockLogic instance
--- @return ComputerBlockLogic
function ComputerBlockLogic.new_simple() end

--- Return ComputerBlockLogic class object
--- @return Class
function ComputerBlockLogic.get_class() end

--- Trying to cast Object into ComputerBlockLogic
--- @param object Object to cast
--- @return ComputerBlockLogic
function ComputerBlockLogic.cast(object) end

--- 
--- 
--- @class ConductorBlockLogic : StorageBlockLogic
--- @field side_cover StaticCover undocumented
--- @field center_cover StaticCover undocumented
--- @field channel string undocumented
--- @field conductor_channel integer undocumented
ConductorBlockLogic = {}

---Add side wire
---@param acc ResourceAccessor
 function ConductorBlockLogic:add_wire(acc) end

--- Creates a new ConductorBlockLogic instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return ConductorBlockLogic
function ConductorBlockLogic.new(parent, name) end

--- Creates a new ConductorBlockLogic instance
--- @return ConductorBlockLogic
function ConductorBlockLogic.new_simple() end

--- Return ConductorBlockLogic class object
--- @return Class
function ConductorBlockLogic.get_class() end

--- Trying to cast Object into ConductorBlockLogic
--- @param object Object to cast
--- @return ConductorBlockLogic
function ConductorBlockLogic.cast(object) end

--- Engine console
--- 
--- @class Console : Instance
Console = {}

--- Executes engine console command
--- @param command string
 function Console.run(command) end

--- Get engine console variable
--- @param variable string
--- @return string
 function Console.get_string(variable) end

--- Get engine console variable
--- @param variable string
--- @return number
 function Console.get_float(variable) end

--- Get engine console variable
--- @param variable string
--- @return integer
 function Console.get_int(variable) end

--- Creates a new EngineConsole instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return EngineConsole
function EngineConsole.new(parent, name) end

--- Creates a new EngineConsole instance
--- @return EngineConsole
function EngineConsole.new_simple() end

--- Return EngineConsole class object
--- @return Class
function EngineConsole.get_class() end

--- Trying to cast Object into EngineConsole
--- @param object Object to cast
--- @return EngineConsole
function EngineConsole.cast(object) end

--- Comment to class
--- 
--- @class DB : Instance
DB = {}

---Register Prototype in DB
---@param proto Prototype Prototype to register
 function DB:reg(proto) end

---Remove Prototype from DB
---@param proto Prototype Prototype to remove
---@return boolean was Prototype removed
 function DB:remove(proto) end

---Register object with class "class" and name "name" from table, filling all other properties from from
table too
---@param table table Object table
 function DB:from_table(table) end

---Register mod table
---@param table table Mod table
 function DB:mod(table) end

---Return all registered objects
---@return Object[]
 function DB:objects() end

--- Creates a new DB instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return DB
function DB.new(parent, name) end

--- Creates a new DB instance
--- @return DB
function DB.new_simple() end

--- Return DB class object
--- @return Class
function DB.get_class() end

--- Trying to cast Object into DB
--- @param object Object to cast
--- @return DB
function DB.cast(object) end

--- 
--- 
--- @class Dimension : Actor
Dimension = {}

---Spawn a block at the given position
---@param bpos Vec3i Block position
---@param quat Quat Rotation
---@param staticBlock StaticBlock Block type to spawn
---@return BlockLogic The spawned block logic instance
 function Dimension:spawn_block(bpos, quat, staticBlock) end

---Spawn a block with full identity at the given position
---@param bpos Vec3i Block position
---@param staticBlock StaticBlock Block type to spawn with identity rotation
---@return BlockLogic The spawned block logic instance
 function Dimension:spawn_block_identity(bpos, staticBlock) end

---Set a block cell at the given position
---@param bpos Vec3i Block position
---@param cl StaticBlock|nil Block type to set
 function Dimension:set_cell(bpos, cl) end

---Clear all props at the given position
---@param bpos Vec3i Block position
 function Dimension:clear_props(bpos) end

---Sample terrain height at XY in world-cell coordinates
---@param x number X coordinate (world cells)
---@param y number Y coordinate (world cells)
---@return number Height in cells
.function Dimension:sample_height(x, y) end

--- 
--- 
--- @class DrillingMachineBase : BlockLogic
--- @field ticks_per_item integer undocumented
--- @field energy_per_tick integer undocumented
--- @field remaining_energy integer undocumented
--- @field productivity integer percent (e.g. 15 = +15%)
--- @field inventory InventoryContainer undocumented
--- @field energy ResourceInventory undocumented
--- @field production integer undocumented
--- @field storage_size integer undocumented
DrillingMachineBase = {}

--- Creates a new DrillingMachineBase instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return DrillingMachineBase
function DrillingMachineBase.new(parent, name) end

--- Creates a new DrillingMachineBase instance
--- @return DrillingMachineBase
function DrillingMachineBase.new_simple() end

--- Return DrillingMachineBase class object
--- @return Class
function DrillingMachineBase.get_class() end

--- Trying to cast Object into DrillingMachineBase
--- @param object Object to cast
--- @return DrillingMachineBase
function DrillingMachineBase.cast(object) end

--- 
--- 
--- @class DrillingRig : DrillingMachineBase
--- @field source SourceData undocumented
DrillingRig = {}

--- Creates a new DrillingRig instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return DrillingRig
function DrillingRig.new(parent, name) end

--- Creates a new DrillingRig instance
--- @return DrillingRig
function DrillingRig.new_simple() end

--- Return DrillingRig class object
--- @return Class
function DrillingRig.get_class() end

--- Trying to cast Object into DrillingRig
--- @param object Object to cast
--- @return DrillingRig
function DrillingRig.cast(object) end

--- 
--- 
--- @class EngineData : Instance
--- @field props_mul number undocumented
--- @field dpi number undocumented
--- @field mouse_sensitivity_x number undocumented
--- @field mouse_sensitivity_y number undocumented
--- @field mouse_inversion_x boolean undocumented
--- @field mouse_inversion_y boolean undocumented
--- @field props_quality number undocumented
--- @field reflection_preset integer undocumented
--- @field transparency_preset integer undocumented
--- @field gi_preset integer undocumented
--- @field detail_shadows boolean undocumented
--- @field fps integer undocumented
--- @field res_x integer undocumented
--- @field res_y integer undocumented
--- @field loading_range integer undocumented
--- @field performance boolean undocumented
--- @field performance_graph boolean undocumented
--- @field ctrl_hotbar boolean undocumented
--- @field alt_hotbar boolean undocumented
--- @field shift_hotbar boolean undocumented
--- @field fov number undocumented
--- @field fog number undocumented
--- @field window_mode integer undocumented
--- @field autosave_period integer undocumented
EngineData = {}

function EngineData:apply() end

function EngineData:show_confirmation() end

--- Creates a new EngineData instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return EngineData
function EngineData.new(parent, name) end

--- Creates a new EngineData instance
--- @return EngineData
function EngineData.new_simple() end

--- Return EngineData class object
--- @return Class
function EngineData.get_class() end

--- Trying to cast Object into EngineData
--- @param object Object to cast
--- @return EngineData
function EngineData.cast(object) end

--- 
--- 
--- @class EventSystem : Object
EventSystem = {}

--- 
--- 
--- @class ExtractionData : Object
--- @field item StaticItem undocumented
--- @field speed integer undocumented
ExtractionData = {}

--- Create new instance of ExtractionData
--- @return ExtractionData
 function ExtractionData.new() end

--- 
--- 
--- @class FluidContainerBlockLogic : ConductorBlockLogic
--- @field capacity integer undocumented
--- @field charge integer undocumented
FluidContainerBlockLogic = {}

--- Creates a new FluidContainerBlockLogic instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return FluidContainerBlockLogic
function FluidContainerBlockLogic.new(parent, name) end

--- Creates a new FluidContainerBlockLogic instance
--- @return FluidContainerBlockLogic
function FluidContainerBlockLogic.new_simple() end

--- Return FluidContainerBlockLogic class object
--- @return Class
function FluidContainerBlockLogic.get_class() end

--- Trying to cast Object into FluidContainerBlockLogic
--- @param object Object to cast
--- @return FluidContainerBlockLogic
function FluidContainerBlockLogic.cast(object) end

--- 
--- 
--- @class Game : Object
--- @field engine_data EngineData undocumented
--- @field mapgen_data MapgenData undocumented
--- @field build_string string undocumented
Game = {}

--- 
--- 
--- @class GameSessionData : Instance
--- @field infinite_ore boolean undocumented
--- @field all_research_completed boolean undocumented
--- @field creative_mode boolean undocumented
--- @field creative_allowed boolean undocumented
--- @field infinite_ore boolean undocumented
--- @field total_game_time double undocumented
--- @field total_game_ticks integer undocumented
--- @field seed string undocumented
--- @field version string undocumented
--- @field generator string undocumented
--- @field save_name string undocumented
--- @field cloud boolean undocumented
GameSessionData = {}

--- Creates a new GameSessionData instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return GameSessionData
function GameSessionData.new(parent, name) end

--- Creates a new GameSessionData instance
--- @return GameSessionData
function GameSessionData.new_simple() end

--- Return GameSessionData class object
--- @return Class
function GameSessionData.get_class() end

--- Trying to cast Object into GameSessionData
--- @param object Object to cast
--- @return GameSessionData
function GameSessionData.cast(object) end

--- 
--- 
--- @class GlobalBiomeFamily : BiomeFamily
GlobalBiomeFamily = {}

--- Register a new GlobalBiomeFamily static object
--- @param name string The name of the object
--- @return GlobalBiomeFamily
function GlobalBiomeFamily.reg(name) end

--- Searching for GlobalBiomeFamily in db
--- @param name string The name of the object
--- @return GlobalBiomeFamily
function GlobalBiomeFamily.find(name) end

--- Return GlobalBiomeFamily class object
--- @return Class
function GlobalBiomeFamily.get_class() end

--- Trying to cast Object into GlobalBiomeFamily
--- @param object Object to cast
--- @return GlobalBiomeFamily
function GlobalBiomeFamily.cast(object) end

--- 
--- 
--- @class HeightGenerator : Prototype
HeightGenerator = {}

---Add a noise generator to the height generator
---@param noise NoiseGenerator Noise generator to add
 function HeightGenerator:add_noise(noise) end

--- Register a new HeightGenerator static object
--- @param name string The name of the object
--- @return HeightGenerator
function HeightGenerator.reg(name) end

--- Searching for HeightGenerator in db
--- @param name string The name of the object
--- @return HeightGenerator
function HeightGenerator.find(name) end

--- Return HeightGenerator class object
--- @return Class
function HeightGenerator.get_class() end

--- Trying to cast Object into HeightGenerator
--- @param object Object to cast
--- @return HeightGenerator
function HeightGenerator.cast(object) end

--- 
--- 
--- @class Inventory : BaseInventory
Inventory = {}

--- Creates a new Inventory instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return Inventory
function Inventory.new(parent, name) end

--- Creates a new Inventory instance
--- @return Inventory
function Inventory.new_simple() end

--- Return Inventory class object
--- @return Class
function Inventory.get_class() end

--- Trying to cast Object into Inventory
--- @param object Object to cast
--- @return Inventory
function Inventory.cast(object) end

--- 
--- 
--- @class InventoryAccess : InventoryReader
--- @field zero_slots boolean undocumented
InventoryAccess = {}

--- Add item with count to InventoryAccess
--- @param item StaticItem
--- @param count integer
--- @return integer Remainder
 function InventoryAccess:add(item, count) end

--- Remove item with count from InventoryAccess
--- @param item StaticItem
--- @param count integer
--- @return integer Remainder
 function InventoryAccess:sub(item, count) end

--- Creates a new InventoryAccess instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return InventoryAccess
function InventoryAccess.new(parent, name) end

--- Creates a new InventoryAccess instance
--- @return InventoryAccess
function InventoryAccess.new_simple() end

--- Return InventoryAccess class object
--- @return Class
function InventoryAccess.get_class() end

--- Trying to cast Object into InventoryAccess
--- @param object Object to cast
--- @return InventoryAccess
function InventoryAccess.cast(object) end

--- 
--- 
--- @class InventoryBlackFilter : InventoryFilter
InventoryBlackFilter = {}

--- Creates a new InventoryBlackFilter instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return InventoryBlackFilter
function InventoryBlackFilter.new(parent, name) end

--- Creates a new InventoryBlackFilter instance
--- @return InventoryBlackFilter
function InventoryBlackFilter.new_simple() end

--- Return InventoryBlackFilter class object
--- @return Class
function InventoryBlackFilter.get_class() end

--- Trying to cast Object into InventoryBlackFilter
--- @param object Object to cast
--- @return InventoryBlackFilter
function InventoryBlackFilter.cast(object) end

--- 
--- 
--- @class InventoryContainer : InventoryAccess
InventoryContainer = {}

---@field index integer
---@return InventoryAccess
 function InventoryContainer:get_access(index) end

---@field inventory InventoryAccess
 function InventoryContainer:bind(inventory) end

--- Creates a new InventoryContainer instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return InventoryContainer
function InventoryContainer.new(parent, name) end

--- Creates a new InventoryContainer instance
--- @return InventoryContainer
function InventoryContainer.new_simple() end

--- Return InventoryContainer class object
--- @return Class
function InventoryContainer.get_class() end

--- Trying to cast Object into InventoryContainer
--- @param object Object to cast
--- @return InventoryContainer
function InventoryContainer.cast(object) end

--- 
--- 
--- @class InventoryFilter : Instance
InventoryFilter = {}

--- Creates a new InventoryFilter instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return InventoryFilter
function InventoryFilter.new(parent, name) end

--- Creates a new InventoryFilter instance
--- @return InventoryFilter
function InventoryFilter.new_simple() end

--- Return InventoryFilter class object
--- @return Class
function InventoryFilter.get_class() end

--- Trying to cast Object into InventoryFilter
--- @param object Object to cast
--- @return InventoryFilter
function InventoryFilter.cast(object) end

--- 
--- 
--- @class InventoryInventoryFilter : InventoryFilter
--- @field inventory InventoryAccess (nil by default)
--- @field is_filtering boolean is filtering enabled (true by default)
--- @field is_white boolean is white filtering enabled (true by default)
InventoryInventoryFilter = {}

--- Creates a new InventoryInventoryFilter instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return InventoryInventoryFilter
function InventoryInventoryFilter.new(parent, name) end

--- Creates a new InventoryInventoryFilter instance
--- @return InventoryInventoryFilter
function InventoryInventoryFilter.new_simple() end

--- Return InventoryInventoryFilter class object
--- @return Class
function InventoryInventoryFilter.get_class() end

--- Trying to cast Object into InventoryInventoryFilter
--- @param object Object to cast
--- @return InventoryInventoryFilter
function InventoryInventoryFilter.cast(object) end

--- 
--- 
--- @class InventoryReader : Instance
--- @field size integer Number of slots in this InventoryReader
InventoryReader = {}

--- Get ItemData with index from InventoryReader
--- @param index integer
--- @return ItemData
 function InventoryReader:get(index) end

--- Looking for index of StaticItem in InventoryReader
--- @param item StaticItem
--- @return integer index of found item or -1
 function InventoryReader:find(item) end

--- Creates a new InventoryReader instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return InventoryReader
function InventoryReader.new(parent, name) end

--- Creates a new InventoryReader instance
--- @return InventoryReader
function InventoryReader.new_simple() end

--- Return InventoryReader class object
--- @return Class
function InventoryReader.get_class() end

--- Trying to cast Object into InventoryReader
--- @param object Object to cast
--- @return InventoryReader
function InventoryReader.cast(object) end

--- 
--- 
--- @class InventoryWhiteFilter : InventoryFilter
InventoryWhiteFilter = {}

--- Creates a new InventoryWhiteFilter instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return InventoryWhiteFilter
function InventoryWhiteFilter.new(parent, name) end

--- Creates a new InventoryWhiteFilter instance
--- @return InventoryWhiteFilter
function InventoryWhiteFilter.new_simple() end

--- Return InventoryWhiteFilter class object
--- @return Class
function InventoryWhiteFilter.get_class() end

--- Trying to cast Object into InventoryWhiteFilter
--- @param object Object to cast
--- @return InventoryWhiteFilter
function InventoryWhiteFilter.cast(object) end

--- 
--- 
--- @class ItemData : Struct
--- @field count integer undocumented
--- @field item StaticItem undocumented
ItemData = {}

--- 
--- 
--- @class LayeringGenerator : Prototype
LayeringGenerator = {}

--- Register a new LayeringGenerator static object
--- @param name string The name of the object
--- @return LayeringGenerator
function LayeringGenerator.reg(name) end

--- Searching for LayeringGenerator in db
--- @param name string The name of the object
--- @return LayeringGenerator
function LayeringGenerator.find(name) end

--- Return LayeringGenerator class object
--- @return Class
function LayeringGenerator.get_class() end

--- Trying to cast Object into LayeringGenerator
--- @param object Object to cast
--- @return LayeringGenerator
function LayeringGenerator.cast(object) end

--- Register a new SimpleLayeringGenerator static object
--- @param name string The name of the object
--- @return SimpleLayeringGenerator
function SimpleLayeringGenerator.reg(name) end

--- Searching for SimpleLayeringGenerator in db
--- @param name string The name of the object
--- @return SimpleLayeringGenerator
function SimpleLayeringGenerator.find(name) end

--- Return SimpleLayeringGenerator class object
--- @return Class
function SimpleLayeringGenerator.get_class() end

--- Trying to cast Object into SimpleLayeringGenerator
--- @param object Object to cast
--- @return SimpleLayeringGenerator
function SimpleLayeringGenerator.cast(object) end

--- Register a new ChancedLayeringGenerator static object
--- @param name string The name of the object
--- @return ChancedLayeringGenerator
function ChancedLayeringGenerator.reg(name) end

--- Searching for ChancedLayeringGenerator in db
--- @param name string The name of the object
--- @return ChancedLayeringGenerator
function ChancedLayeringGenerator.find(name) end

--- Return ChancedLayeringGenerator class object
--- @return Class
function ChancedLayeringGenerator.get_class() end

--- Trying to cast Object into ChancedLayeringGenerator
--- @param object Object to cast
--- @return ChancedLayeringGenerator
function ChancedLayeringGenerator.cast(object) end

--- 
--- 
--- @class LogicExportOption : Prototype
--- @field signal StaticItem Signal
--- @field enabled bool Enabled
--- @field label FLoc Label
--- @field tooltip FLoc Tooltip
LogicExportOption = {}

--- Register a new LogicExportOption static object
--- @param name string The name of the object
--- @return LogicExportOption
function LogicExportOption.reg(name) end

--- Searching for LogicExportOption in db
--- @param name string The name of the object
--- @return LogicExportOption
function LogicExportOption.find(name) end

--- Return LogicExportOption class object
--- @return Class
function LogicExportOption.get_class() end

--- Trying to cast Object into LogicExportOption
--- @param object Object to cast
--- @return LogicExportOption
function LogicExportOption.cast(object) end

--- 
--- 
--- @class LogicNode : LogicNode
LogicNode = {}

--- Creates a new LogicNode instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode
function LogicNode.new(parent, name) end

--- Creates a new LogicNode instance
--- @return LogicNode
function LogicNode.new_simple() end

--- Return LogicNode class object
--- @return Class
function LogicNode.get_class() end

--- Trying to cast Object into LogicNode
--- @param object Object to cast
--- @return LogicNode
function LogicNode.cast(object) end

--- Creates a new LogicNode instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode
function LogicNode.new(parent, name) end

--- Creates a new LogicNode instance
--- @return LogicNode
function LogicNode.new_simple() end

--- Return LogicNode class object
--- @return Class
function LogicNode.get_class() end

--- Trying to cast Object into LogicNode
--- @param object Object to cast
--- @return LogicNode
function LogicNode.cast(object) end

--- Creates a new LogicNode_Arithmetic instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode_Arithmetic
function LogicNode_Arithmetic.new(parent, name) end

--- Creates a new LogicNode_Arithmetic instance
--- @return LogicNode_Arithmetic
function LogicNode_Arithmetic.new_simple() end

--- Return LogicNode_Arithmetic class object
--- @return Class
function LogicNode_Arithmetic.get_class() end

--- Trying to cast Object into LogicNode_Arithmetic
--- @param object Object to cast
--- @return LogicNode_Arithmetic
function LogicNode_Arithmetic.cast(object) end

--- Creates a new LogicNode_Decider instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode_Decider
function LogicNode_Decider.new(parent, name) end

--- Creates a new LogicNode_Decider instance
--- @return LogicNode_Decider
function LogicNode_Decider.new_simple() end

--- Return LogicNode_Decider class object
--- @return Class
function LogicNode_Decider.get_class() end

--- Trying to cast Object into LogicNode_Decider
--- @param object Object to cast
--- @return LogicNode_Decider
function LogicNode_Decider.cast(object) end

--- Creates a new LogicNode_Latch instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode_Latch
function LogicNode_Latch.new(parent, name) end

--- Creates a new LogicNode_Latch instance
--- @return LogicNode_Latch
function LogicNode_Latch.new_simple() end

--- Return LogicNode_Latch class object
--- @return Class
function LogicNode_Latch.get_class() end

--- Trying to cast Object into LogicNode_Latch
--- @param object Object to cast
--- @return LogicNode_Latch
function LogicNode_Latch.cast(object) end

--- Creates a new LogicNode_ReadMachine instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode_ReadMachine
function LogicNode_ReadMachine.new(parent, name) end

--- Creates a new LogicNode_ReadMachine instance
--- @return LogicNode_ReadMachine
function LogicNode_ReadMachine.new_simple() end

--- Return LogicNode_ReadMachine class object
--- @return Class
function LogicNode_ReadMachine.get_class() end

--- Trying to cast Object into LogicNode_ReadMachine
--- @param object Object to cast
--- @return LogicNode_ReadMachine
function LogicNode_ReadMachine.cast(object) end

--- Creates a new LogicNode_ControlMachine instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode_ControlMachine
function LogicNode_ControlMachine.new(parent, name) end

--- Creates a new LogicNode_ControlMachine instance
--- @return LogicNode_ControlMachine
function LogicNode_ControlMachine.new_simple() end

--- Return LogicNode_ControlMachine class object
--- @return Class
function LogicNode_ControlMachine.get_class() end

--- Trying to cast Object into LogicNode_ControlMachine
--- @param object Object to cast
--- @return LogicNode_ControlMachine
function LogicNode_ControlMachine.cast(object) end

--- Creates a new LogicProgram instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicProgram
function LogicProgram.new(parent, name) end

--- Creates a new LogicProgram instance
--- @return LogicProgram
function LogicProgram.new_simple() end

--- Return LogicProgram class object
--- @return Class
function LogicProgram.get_class() end

--- Trying to cast Object into LogicProgram
--- @param object Object to cast
--- @return LogicProgram
function LogicProgram.cast(object) end

--- 
--- 
--- @class LogicNode_Arithmetic : LogicNode
LogicNode_Arithmetic = {}

--- Creates a new LogicNode instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode
function LogicNode.new(parent, name) end

--- Creates a new LogicNode instance
--- @return LogicNode
function LogicNode.new_simple() end

--- Return LogicNode class object
--- @return Class
function LogicNode.get_class() end

--- Trying to cast Object into LogicNode
--- @param object Object to cast
--- @return LogicNode
function LogicNode.cast(object) end

--- Creates a new LogicNode instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode
function LogicNode.new(parent, name) end

--- Creates a new LogicNode instance
--- @return LogicNode
function LogicNode.new_simple() end

--- Return LogicNode class object
--- @return Class
function LogicNode.get_class() end

--- Trying to cast Object into LogicNode
--- @param object Object to cast
--- @return LogicNode
function LogicNode.cast(object) end

--- Creates a new LogicNode_Arithmetic instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode_Arithmetic
function LogicNode_Arithmetic.new(parent, name) end

--- Creates a new LogicNode_Arithmetic instance
--- @return LogicNode_Arithmetic
function LogicNode_Arithmetic.new_simple() end

--- Return LogicNode_Arithmetic class object
--- @return Class
function LogicNode_Arithmetic.get_class() end

--- Trying to cast Object into LogicNode_Arithmetic
--- @param object Object to cast
--- @return LogicNode_Arithmetic
function LogicNode_Arithmetic.cast(object) end

--- Creates a new LogicNode_Decider instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode_Decider
function LogicNode_Decider.new(parent, name) end

--- Creates a new LogicNode_Decider instance
--- @return LogicNode_Decider
function LogicNode_Decider.new_simple() end

--- Return LogicNode_Decider class object
--- @return Class
function LogicNode_Decider.get_class() end

--- Trying to cast Object into LogicNode_Decider
--- @param object Object to cast
--- @return LogicNode_Decider
function LogicNode_Decider.cast(object) end

--- Creates a new LogicNode_Latch instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode_Latch
function LogicNode_Latch.new(parent, name) end

--- Creates a new LogicNode_Latch instance
--- @return LogicNode_Latch
function LogicNode_Latch.new_simple() end

--- Return LogicNode_Latch class object
--- @return Class
function LogicNode_Latch.get_class() end

--- Trying to cast Object into LogicNode_Latch
--- @param object Object to cast
--- @return LogicNode_Latch
function LogicNode_Latch.cast(object) end

--- Creates a new LogicNode_ReadMachine instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode_ReadMachine
function LogicNode_ReadMachine.new(parent, name) end

--- Creates a new LogicNode_ReadMachine instance
--- @return LogicNode_ReadMachine
function LogicNode_ReadMachine.new_simple() end

--- Return LogicNode_ReadMachine class object
--- @return Class
function LogicNode_ReadMachine.get_class() end

--- Trying to cast Object into LogicNode_ReadMachine
--- @param object Object to cast
--- @return LogicNode_ReadMachine
function LogicNode_ReadMachine.cast(object) end

--- Creates a new LogicNode_ControlMachine instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode_ControlMachine
function LogicNode_ControlMachine.new(parent, name) end

--- Creates a new LogicNode_ControlMachine instance
--- @return LogicNode_ControlMachine
function LogicNode_ControlMachine.new_simple() end

--- Return LogicNode_ControlMachine class object
--- @return Class
function LogicNode_ControlMachine.get_class() end

--- Trying to cast Object into LogicNode_ControlMachine
--- @param object Object to cast
--- @return LogicNode_ControlMachine
function LogicNode_ControlMachine.cast(object) end

--- Creates a new LogicProgram instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicProgram
function LogicProgram.new(parent, name) end

--- Creates a new LogicProgram instance
--- @return LogicProgram
function LogicProgram.new_simple() end

--- Return LogicProgram class object
--- @return Class
function LogicProgram.get_class() end

--- Trying to cast Object into LogicProgram
--- @param object Object to cast
--- @return LogicProgram
function LogicProgram.cast(object) end

--- 
--- 
--- @class LogicNode_ControlMachine : LogicNode
LogicNode_ControlMachine = {}

--- Creates a new LogicNode instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode
function LogicNode.new(parent, name) end

--- Creates a new LogicNode instance
--- @return LogicNode
function LogicNode.new_simple() end

--- Return LogicNode class object
--- @return Class
function LogicNode.get_class() end

--- Trying to cast Object into LogicNode
--- @param object Object to cast
--- @return LogicNode
function LogicNode.cast(object) end

--- Creates a new LogicNode instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode
function LogicNode.new(parent, name) end

--- Creates a new LogicNode instance
--- @return LogicNode
function LogicNode.new_simple() end

--- Return LogicNode class object
--- @return Class
function LogicNode.get_class() end

--- Trying to cast Object into LogicNode
--- @param object Object to cast
--- @return LogicNode
function LogicNode.cast(object) end

--- Creates a new LogicNode_Arithmetic instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode_Arithmetic
function LogicNode_Arithmetic.new(parent, name) end

--- Creates a new LogicNode_Arithmetic instance
--- @return LogicNode_Arithmetic
function LogicNode_Arithmetic.new_simple() end

--- Return LogicNode_Arithmetic class object
--- @return Class
function LogicNode_Arithmetic.get_class() end

--- Trying to cast Object into LogicNode_Arithmetic
--- @param object Object to cast
--- @return LogicNode_Arithmetic
function LogicNode_Arithmetic.cast(object) end

--- Creates a new LogicNode_Decider instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode_Decider
function LogicNode_Decider.new(parent, name) end

--- Creates a new LogicNode_Decider instance
--- @return LogicNode_Decider
function LogicNode_Decider.new_simple() end

--- Return LogicNode_Decider class object
--- @return Class
function LogicNode_Decider.get_class() end

--- Trying to cast Object into LogicNode_Decider
--- @param object Object to cast
--- @return LogicNode_Decider
function LogicNode_Decider.cast(object) end

--- Creates a new LogicNode_Latch instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode_Latch
function LogicNode_Latch.new(parent, name) end

--- Creates a new LogicNode_Latch instance
--- @return LogicNode_Latch
function LogicNode_Latch.new_simple() end

--- Return LogicNode_Latch class object
--- @return Class
function LogicNode_Latch.get_class() end

--- Trying to cast Object into LogicNode_Latch
--- @param object Object to cast
--- @return LogicNode_Latch
function LogicNode_Latch.cast(object) end

--- Creates a new LogicNode_ReadMachine instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode_ReadMachine
function LogicNode_ReadMachine.new(parent, name) end

--- Creates a new LogicNode_ReadMachine instance
--- @return LogicNode_ReadMachine
function LogicNode_ReadMachine.new_simple() end

--- Return LogicNode_ReadMachine class object
--- @return Class
function LogicNode_ReadMachine.get_class() end

--- Trying to cast Object into LogicNode_ReadMachine
--- @param object Object to cast
--- @return LogicNode_ReadMachine
function LogicNode_ReadMachine.cast(object) end

--- Creates a new LogicNode_ControlMachine instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode_ControlMachine
function LogicNode_ControlMachine.new(parent, name) end

--- Creates a new LogicNode_ControlMachine instance
--- @return LogicNode_ControlMachine
function LogicNode_ControlMachine.new_simple() end

--- Return LogicNode_ControlMachine class object
--- @return Class
function LogicNode_ControlMachine.get_class() end

--- Trying to cast Object into LogicNode_ControlMachine
--- @param object Object to cast
--- @return LogicNode_ControlMachine
function LogicNode_ControlMachine.cast(object) end

--- Creates a new LogicProgram instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicProgram
function LogicProgram.new(parent, name) end

--- Creates a new LogicProgram instance
--- @return LogicProgram
function LogicProgram.new_simple() end

--- Return LogicProgram class object
--- @return Class
function LogicProgram.get_class() end

--- Trying to cast Object into LogicProgram
--- @param object Object to cast
--- @return LogicProgram
function LogicProgram.cast(object) end

--- 
--- 
--- @class LogicNode_Decider : LogicNode
LogicNode_Decider = {}

--- Creates a new LogicNode instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode
function LogicNode.new(parent, name) end

--- Creates a new LogicNode instance
--- @return LogicNode
function LogicNode.new_simple() end

--- Return LogicNode class object
--- @return Class
function LogicNode.get_class() end

--- Trying to cast Object into LogicNode
--- @param object Object to cast
--- @return LogicNode
function LogicNode.cast(object) end

--- Creates a new LogicNode instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode
function LogicNode.new(parent, name) end

--- Creates a new LogicNode instance
--- @return LogicNode
function LogicNode.new_simple() end

--- Return LogicNode class object
--- @return Class
function LogicNode.get_class() end

--- Trying to cast Object into LogicNode
--- @param object Object to cast
--- @return LogicNode
function LogicNode.cast(object) end

--- Creates a new LogicNode_Arithmetic instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode_Arithmetic
function LogicNode_Arithmetic.new(parent, name) end

--- Creates a new LogicNode_Arithmetic instance
--- @return LogicNode_Arithmetic
function LogicNode_Arithmetic.new_simple() end

--- Return LogicNode_Arithmetic class object
--- @return Class
function LogicNode_Arithmetic.get_class() end

--- Trying to cast Object into LogicNode_Arithmetic
--- @param object Object to cast
--- @return LogicNode_Arithmetic
function LogicNode_Arithmetic.cast(object) end

--- Creates a new LogicNode_Decider instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode_Decider
function LogicNode_Decider.new(parent, name) end

--- Creates a new LogicNode_Decider instance
--- @return LogicNode_Decider
function LogicNode_Decider.new_simple() end

--- Return LogicNode_Decider class object
--- @return Class
function LogicNode_Decider.get_class() end

--- Trying to cast Object into LogicNode_Decider
--- @param object Object to cast
--- @return LogicNode_Decider
function LogicNode_Decider.cast(object) end

--- Creates a new LogicNode_Latch instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode_Latch
function LogicNode_Latch.new(parent, name) end

--- Creates a new LogicNode_Latch instance
--- @return LogicNode_Latch
function LogicNode_Latch.new_simple() end

--- Return LogicNode_Latch class object
--- @return Class
function LogicNode_Latch.get_class() end

--- Trying to cast Object into LogicNode_Latch
--- @param object Object to cast
--- @return LogicNode_Latch
function LogicNode_Latch.cast(object) end

--- Creates a new LogicNode_ReadMachine instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode_ReadMachine
function LogicNode_ReadMachine.new(parent, name) end

--- Creates a new LogicNode_ReadMachine instance
--- @return LogicNode_ReadMachine
function LogicNode_ReadMachine.new_simple() end

--- Return LogicNode_ReadMachine class object
--- @return Class
function LogicNode_ReadMachine.get_class() end

--- Trying to cast Object into LogicNode_ReadMachine
--- @param object Object to cast
--- @return LogicNode_ReadMachine
function LogicNode_ReadMachine.cast(object) end

--- Creates a new LogicNode_ControlMachine instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode_ControlMachine
function LogicNode_ControlMachine.new(parent, name) end

--- Creates a new LogicNode_ControlMachine instance
--- @return LogicNode_ControlMachine
function LogicNode_ControlMachine.new_simple() end

--- Return LogicNode_ControlMachine class object
--- @return Class
function LogicNode_ControlMachine.get_class() end

--- Trying to cast Object into LogicNode_ControlMachine
--- @param object Object to cast
--- @return LogicNode_ControlMachine
function LogicNode_ControlMachine.cast(object) end

--- Creates a new LogicProgram instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicProgram
function LogicProgram.new(parent, name) end

--- Creates a new LogicProgram instance
--- @return LogicProgram
function LogicProgram.new_simple() end

--- Return LogicProgram class object
--- @return Class
function LogicProgram.get_class() end

--- Trying to cast Object into LogicProgram
--- @param object Object to cast
--- @return LogicProgram
function LogicProgram.cast(object) end

--- 
--- 
--- @class LogicNode_Latch : LogicNode
LogicNode_Latch = {}

--- Creates a new LogicNode instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode
function LogicNode.new(parent, name) end

--- Creates a new LogicNode instance
--- @return LogicNode
function LogicNode.new_simple() end

--- Return LogicNode class object
--- @return Class
function LogicNode.get_class() end

--- Trying to cast Object into LogicNode
--- @param object Object to cast
--- @return LogicNode
function LogicNode.cast(object) end

--- Creates a new LogicNode instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode
function LogicNode.new(parent, name) end

--- Creates a new LogicNode instance
--- @return LogicNode
function LogicNode.new_simple() end

--- Return LogicNode class object
--- @return Class
function LogicNode.get_class() end

--- Trying to cast Object into LogicNode
--- @param object Object to cast
--- @return LogicNode
function LogicNode.cast(object) end

--- Creates a new LogicNode_Arithmetic instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode_Arithmetic
function LogicNode_Arithmetic.new(parent, name) end

--- Creates a new LogicNode_Arithmetic instance
--- @return LogicNode_Arithmetic
function LogicNode_Arithmetic.new_simple() end

--- Return LogicNode_Arithmetic class object
--- @return Class
function LogicNode_Arithmetic.get_class() end

--- Trying to cast Object into LogicNode_Arithmetic
--- @param object Object to cast
--- @return LogicNode_Arithmetic
function LogicNode_Arithmetic.cast(object) end

--- Creates a new LogicNode_Decider instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode_Decider
function LogicNode_Decider.new(parent, name) end

--- Creates a new LogicNode_Decider instance
--- @return LogicNode_Decider
function LogicNode_Decider.new_simple() end

--- Return LogicNode_Decider class object
--- @return Class
function LogicNode_Decider.get_class() end

--- Trying to cast Object into LogicNode_Decider
--- @param object Object to cast
--- @return LogicNode_Decider
function LogicNode_Decider.cast(object) end

--- Creates a new LogicNode_Latch instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode_Latch
function LogicNode_Latch.new(parent, name) end

--- Creates a new LogicNode_Latch instance
--- @return LogicNode_Latch
function LogicNode_Latch.new_simple() end

--- Return LogicNode_Latch class object
--- @return Class
function LogicNode_Latch.get_class() end

--- Trying to cast Object into LogicNode_Latch
--- @param object Object to cast
--- @return LogicNode_Latch
function LogicNode_Latch.cast(object) end

--- Creates a new LogicNode_ReadMachine instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode_ReadMachine
function LogicNode_ReadMachine.new(parent, name) end

--- Creates a new LogicNode_ReadMachine instance
--- @return LogicNode_ReadMachine
function LogicNode_ReadMachine.new_simple() end

--- Return LogicNode_ReadMachine class object
--- @return Class
function LogicNode_ReadMachine.get_class() end

--- Trying to cast Object into LogicNode_ReadMachine
--- @param object Object to cast
--- @return LogicNode_ReadMachine
function LogicNode_ReadMachine.cast(object) end

--- Creates a new LogicNode_ControlMachine instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode_ControlMachine
function LogicNode_ControlMachine.new(parent, name) end

--- Creates a new LogicNode_ControlMachine instance
--- @return LogicNode_ControlMachine
function LogicNode_ControlMachine.new_simple() end

--- Return LogicNode_ControlMachine class object
--- @return Class
function LogicNode_ControlMachine.get_class() end

--- Trying to cast Object into LogicNode_ControlMachine
--- @param object Object to cast
--- @return LogicNode_ControlMachine
function LogicNode_ControlMachine.cast(object) end

--- Creates a new LogicProgram instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicProgram
function LogicProgram.new(parent, name) end

--- Creates a new LogicProgram instance
--- @return LogicProgram
function LogicProgram.new_simple() end

--- Return LogicProgram class object
--- @return Class
function LogicProgram.get_class() end

--- Trying to cast Object into LogicProgram
--- @param object Object to cast
--- @return LogicProgram
function LogicProgram.cast(object) end

--- 
--- 
--- @class LogicNode_ReadMachine : LogicNode
LogicNode_ReadMachine = {}

--- Creates a new LogicNode instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode
function LogicNode.new(parent, name) end

--- Creates a new LogicNode instance
--- @return LogicNode
function LogicNode.new_simple() end

--- Return LogicNode class object
--- @return Class
function LogicNode.get_class() end

--- Trying to cast Object into LogicNode
--- @param object Object to cast
--- @return LogicNode
function LogicNode.cast(object) end

--- Creates a new LogicNode instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode
function LogicNode.new(parent, name) end

--- Creates a new LogicNode instance
--- @return LogicNode
function LogicNode.new_simple() end

--- Return LogicNode class object
--- @return Class
function LogicNode.get_class() end

--- Trying to cast Object into LogicNode
--- @param object Object to cast
--- @return LogicNode
function LogicNode.cast(object) end

--- Creates a new LogicNode_Arithmetic instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode_Arithmetic
function LogicNode_Arithmetic.new(parent, name) end

--- Creates a new LogicNode_Arithmetic instance
--- @return LogicNode_Arithmetic
function LogicNode_Arithmetic.new_simple() end

--- Return LogicNode_Arithmetic class object
--- @return Class
function LogicNode_Arithmetic.get_class() end

--- Trying to cast Object into LogicNode_Arithmetic
--- @param object Object to cast
--- @return LogicNode_Arithmetic
function LogicNode_Arithmetic.cast(object) end

--- Creates a new LogicNode_Decider instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode_Decider
function LogicNode_Decider.new(parent, name) end

--- Creates a new LogicNode_Decider instance
--- @return LogicNode_Decider
function LogicNode_Decider.new_simple() end

--- Return LogicNode_Decider class object
--- @return Class
function LogicNode_Decider.get_class() end

--- Trying to cast Object into LogicNode_Decider
--- @param object Object to cast
--- @return LogicNode_Decider
function LogicNode_Decider.cast(object) end

--- Creates a new LogicNode_Latch instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode_Latch
function LogicNode_Latch.new(parent, name) end

--- Creates a new LogicNode_Latch instance
--- @return LogicNode_Latch
function LogicNode_Latch.new_simple() end

--- Return LogicNode_Latch class object
--- @return Class
function LogicNode_Latch.get_class() end

--- Trying to cast Object into LogicNode_Latch
--- @param object Object to cast
--- @return LogicNode_Latch
function LogicNode_Latch.cast(object) end

--- Creates a new LogicNode_ReadMachine instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode_ReadMachine
function LogicNode_ReadMachine.new(parent, name) end

--- Creates a new LogicNode_ReadMachine instance
--- @return LogicNode_ReadMachine
function LogicNode_ReadMachine.new_simple() end

--- Return LogicNode_ReadMachine class object
--- @return Class
function LogicNode_ReadMachine.get_class() end

--- Trying to cast Object into LogicNode_ReadMachine
--- @param object Object to cast
--- @return LogicNode_ReadMachine
function LogicNode_ReadMachine.cast(object) end

--- Creates a new LogicNode_ControlMachine instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode_ControlMachine
function LogicNode_ControlMachine.new(parent, name) end

--- Creates a new LogicNode_ControlMachine instance
--- @return LogicNode_ControlMachine
function LogicNode_ControlMachine.new_simple() end

--- Return LogicNode_ControlMachine class object
--- @return Class
function LogicNode_ControlMachine.get_class() end

--- Trying to cast Object into LogicNode_ControlMachine
--- @param object Object to cast
--- @return LogicNode_ControlMachine
function LogicNode_ControlMachine.cast(object) end

--- Creates a new LogicProgram instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicProgram
function LogicProgram.new(parent, name) end

--- Creates a new LogicProgram instance
--- @return LogicProgram
function LogicProgram.new_simple() end

--- Return LogicProgram class object
--- @return Class
function LogicProgram.get_class() end

--- Trying to cast Object into LogicProgram
--- @param object Object to cast
--- @return LogicProgram
function LogicProgram.cast(object) end

--- 
--- 
--- @class LogicProgram : LogicNode
LogicProgram = {}

--- Creates a new LogicNode instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode
function LogicNode.new(parent, name) end

--- Creates a new LogicNode instance
--- @return LogicNode
function LogicNode.new_simple() end

--- Return LogicNode class object
--- @return Class
function LogicNode.get_class() end

--- Trying to cast Object into LogicNode
--- @param object Object to cast
--- @return LogicNode
function LogicNode.cast(object) end

--- Creates a new LogicNode instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode
function LogicNode.new(parent, name) end

--- Creates a new LogicNode instance
--- @return LogicNode
function LogicNode.new_simple() end

--- Return LogicNode class object
--- @return Class
function LogicNode.get_class() end

--- Trying to cast Object into LogicNode
--- @param object Object to cast
--- @return LogicNode
function LogicNode.cast(object) end

--- Creates a new LogicNode_Arithmetic instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode_Arithmetic
function LogicNode_Arithmetic.new(parent, name) end

--- Creates a new LogicNode_Arithmetic instance
--- @return LogicNode_Arithmetic
function LogicNode_Arithmetic.new_simple() end

--- Return LogicNode_Arithmetic class object
--- @return Class
function LogicNode_Arithmetic.get_class() end

--- Trying to cast Object into LogicNode_Arithmetic
--- @param object Object to cast
--- @return LogicNode_Arithmetic
function LogicNode_Arithmetic.cast(object) end

--- Creates a new LogicNode_Decider instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode_Decider
function LogicNode_Decider.new(parent, name) end

--- Creates a new LogicNode_Decider instance
--- @return LogicNode_Decider
function LogicNode_Decider.new_simple() end

--- Return LogicNode_Decider class object
--- @return Class
function LogicNode_Decider.get_class() end

--- Trying to cast Object into LogicNode_Decider
--- @param object Object to cast
--- @return LogicNode_Decider
function LogicNode_Decider.cast(object) end

--- Creates a new LogicNode_Latch instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode_Latch
function LogicNode_Latch.new(parent, name) end

--- Creates a new LogicNode_Latch instance
--- @return LogicNode_Latch
function LogicNode_Latch.new_simple() end

--- Return LogicNode_Latch class object
--- @return Class
function LogicNode_Latch.get_class() end

--- Trying to cast Object into LogicNode_Latch
--- @param object Object to cast
--- @return LogicNode_Latch
function LogicNode_Latch.cast(object) end

--- Creates a new LogicNode_ReadMachine instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode_ReadMachine
function LogicNode_ReadMachine.new(parent, name) end

--- Creates a new LogicNode_ReadMachine instance
--- @return LogicNode_ReadMachine
function LogicNode_ReadMachine.new_simple() end

--- Return LogicNode_ReadMachine class object
--- @return Class
function LogicNode_ReadMachine.get_class() end

--- Trying to cast Object into LogicNode_ReadMachine
--- @param object Object to cast
--- @return LogicNode_ReadMachine
function LogicNode_ReadMachine.cast(object) end

--- Creates a new LogicNode_ControlMachine instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicNode_ControlMachine
function LogicNode_ControlMachine.new(parent, name) end

--- Creates a new LogicNode_ControlMachine instance
--- @return LogicNode_ControlMachine
function LogicNode_ControlMachine.new_simple() end

--- Return LogicNode_ControlMachine class object
--- @return Class
function LogicNode_ControlMachine.get_class() end

--- Trying to cast Object into LogicNode_ControlMachine
--- @param object Object to cast
--- @return LogicNode_ControlMachine
function LogicNode_ControlMachine.cast(object) end

--- Creates a new LogicProgram instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicProgram
function LogicProgram.new(parent, name) end

--- Creates a new LogicProgram instance
--- @return LogicProgram
function LogicProgram.new_simple() end

--- Return LogicProgram class object
--- @return Class
function LogicProgram.get_class() end

--- Trying to cast Object into LogicProgram
--- @param object Object to cast
--- @return LogicProgram
function LogicProgram.cast(object) end

--- 
--- 
--- @class LogicSettings : Instance
--- @field export LogicExportOption[] undocumented
LogicSettings = {}

--- Creates a new LogicSettings instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return LogicSettings
function LogicSettings.new(parent, name) end

--- Creates a new LogicSettings instance
--- @return LogicSettings
function LogicSettings.new_simple() end

--- Return LogicSettings class object
--- @return Class
function LogicSettings.get_class() end

--- Trying to cast Object into LogicSettings
--- @param object Object to cast
--- @return LogicSettings
function LogicSettings.cast(object) end

--- 
--- 
--- @class MapStructure : Object
--- @field offset Vec2i undocumented
--- @field structure StaticStructure undocumented
MapStructure = {}

--- Create new instance of MapStructure
--- @return MapStructure
 function MapStructure.new() end

--- 
--- 
--- @class MapgenData : Instance
MapgenData = {}

--- Creates a new MapgenData instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return MapgenData
function MapgenData.new(parent, name) end

--- Creates a new MapgenData instance
--- @return MapgenData
function MapgenData.new_simple() end

--- Return MapgenData class object
--- @return Class
function MapgenData.get_class() end

--- Trying to cast Object into MapgenData
--- @param object Object to cast
--- @return MapgenData
function MapgenData.cast(object) end

--- 
--- 
--- @class NoiseGenerator : Prototype
--- @field min number undocumented
--- @field max number undocumented
NoiseGenerator = {}

---Set the seed for noise generation
---@param seed integer Seed value
 function NoiseGenerator:set_seed(seed) end

---Get the noise value at the given position
---@param x number X coordinate
---@param y number Y coordinate
---@param z number Z coordinate
 function NoiseGenerator:get_noise(x, y, z) end

---Set the noise type
---@param noise_type string Noise type
 function NoiseGenerator:set_noise_type(noise_type) end

---Set the frequency for noise generation
---@param frequency number Frequency value
 function NoiseGenerator:set_frequency(frequency) end

---Set the fractal octaves for noise generation
---@param octaves integer Octaves value
 function NoiseGenerator:set_fractal_octaves(octaves) end

---Set the fractal gain for noise generation
---@param gain number Gain value
 function NoiseGenerator:set_fractal_gain(gain) end

---Set the fractal lacunarity for noise generation
---@param lacunarity number Lacunarity value
 function NoiseGenerator:set_fractal_lacunarity(lacunarity) end

---Set the fractal type for noise generation
---@param fractal_type string Fractal type
 function NoiseGenerator:set_fractal_type(fractal_type) end

--- Register a new NoiseGenerator static object
--- @param name string The name of the object
--- @return NoiseGenerator
function NoiseGenerator.reg(name) end

--- Searching for NoiseGenerator in db
--- @param name string The name of the object
--- @return NoiseGenerator
function NoiseGenerator.find(name) end

--- Return NoiseGenerator class object
--- @return Class
function NoiseGenerator.get_class() end

--- Trying to cast Object into NoiseGenerator
--- @param object Object to cast
--- @return NoiseGenerator
function NoiseGenerator.cast(object) end

--- Structure that stores several StaticProp records (prop variations) with shared spawn chance
--- 
--- @class PropListData : Struct
--- @field weight integer Spawn weight
--- @field props StaticProp[] List of prop assets (prop variations)
PropListData = {}

--- @return PropListData
 function PropListData.new() end

--- 
--- 
--- @class PropsGenerator : Prototype
PropsGenerator = {}

--- Register a new PropsGenerator static object
--- @param name string The name of the object
--- @return PropsGenerator
function PropsGenerator.reg(name) end

--- Searching for PropsGenerator in db
--- @param name string The name of the object
--- @return PropsGenerator
function PropsGenerator.find(name) end

--- Return PropsGenerator class object
--- @return Class
function PropsGenerator.get_class() end

--- Trying to cast Object into PropsGenerator
--- @param object Object to cast
--- @return PropsGenerator
function PropsGenerator.cast(object) end

--- Database record
--- 
--- @class Prototype : Object
--- @field name string Object name
Prototype = {}

--- Creates a new type instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return type
function type.new(parent, name) end

--- Return type class object
--- @return Class
function type.get_class() end

--- Trying to cast Object into type
--- @param object Object to cast
--- @return type
function type.cast(object) end

--- Creates a new type instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return type
function type.new(parent, name) end

--- Creates a new type instance
--- @return type
function type.new_simple() end

--- Return type class object
--- @return Class
function type.get_class() end

--- Trying to cast Object into type
--- @param object Object to cast
--- @return type
function type.cast(object) end

--- Creates a new type instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return type
function type.new(parent, name) end

--- Creates a new type instance
--- @return type
function type.new_simple() end

--- Return type class object
--- @return Class
function type.get_class() end

--- Trying to cast Object into type
--- @param object Object to cast
--- @return type
function type.cast(object) end

--- Register a new type static object
--- @param name string The name of the object
--- @return type
function type.reg(name) end

--- Searching for type in db
--- @param name string The name of the object
--- @return type
function type.find(name) end

--- Return type class object
--- @return Class
function type.get_class() end

--- Trying to cast Object into type
--- @param object Object to cast
--- @return type
function type.cast(object) end

--- Register a new type static object
--- @param name string The name of the object
--- @return type
function type.reg(name) end

--- Searching for type in db
--- @param name string The name of the object
--- @return type
function type.find(name) end

--- Return type class object
--- @return Class
function type.get_class() end

--- Trying to cast Object into type
--- @param object Object to cast
--- @return type
function type.cast(object) end

--- 
--- 
--- @class Pumpjack : DrillingMachineBase
--- @field layer RegionLayer undocumented
Pumpjack = {}

--- Creates a new Pumpjack instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return Pumpjack
function Pumpjack.new(parent, name) end

--- Creates a new Pumpjack instance
--- @return Pumpjack
function Pumpjack.new_simple() end

--- Return Pumpjack class object
--- @return Class
function Pumpjack.get_class() end

--- Trying to cast Object into Pumpjack
--- @param object Object to cast
--- @return Pumpjack
function Pumpjack.cast(object) end

--- Crafting-recipe prototype used by machines and crafting UIs
--- 
--- @class Recipe : Prototype
--- @field ticks integer *Craft time* in engine ticks
--- @field default_locked boolean If **true**, the recipe starts
--- @field locked boolean Currently locked if **true**
--- @field productivity integer Percentage bonus (e.g. `20` = +20 %)
--- @field tier integer Recipe tier used for speed scaling: every tier **above** the
--- @field start_tier integer Machine-unlock tier (same for all its recipes); lets you
Recipe = {}

--- Register a new Recipe static object
--- @param name string The name of the object
--- @return Recipe
function Recipe.reg(name) end

--- Searching for Recipe in db
--- @param name string The name of the object
--- @return Recipe
function Recipe.find(name) end

--- Return Recipe class object
--- @return Class
function Recipe.get_class() end

--- Trying to cast Object into Recipe
--- @param object Object to cast
--- @return Recipe
function Recipe.cast(object) end

--- 
--- 
--- @class RecipeDictionary : Prototype
--- @field start_tier integer undocumented
RecipeDictionary = {}

--- Register a new RecipeDictionary static object
--- @param name string The name of the object
--- @return RecipeDictionary
function RecipeDictionary.reg(name) end

--- Searching for RecipeDictionary in db
--- @param name string The name of the object
--- @return RecipeDictionary
function RecipeDictionary.find(name) end

--- Return RecipeDictionary class object
--- @return Class
function RecipeDictionary.get_class() end

--- Trying to cast Object into RecipeDictionary
--- @param object Object to cast
--- @return RecipeDictionary
function RecipeDictionary.cast(object) end

--- 
--- 
--- @class RecipeInventory : AutosizeInventory
--- @field recipe Recipe undocumented
RecipeInventory = {}

--- Creates a new RecipeInventory instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return RecipeInventory
function RecipeInventory.new(parent, name) end

--- Creates a new RecipeInventory instance
--- @return RecipeInventory
function RecipeInventory.new_simple() end

--- Return RecipeInventory class object
--- @return Class
function RecipeInventory.get_class() end

--- Trying to cast Object into RecipeInventory
--- @param object Object to cast
--- @return RecipeInventory
function RecipeInventory.cast(object) end

--- 
--- 
--- @class Region : Instance
--- @field pos Vec2i undocumented
Region = {}

---Add structure to this Region
---@param structure MapStructure
 function Region:add_structure(structure) end

---Add source to this Region
---@param source SourceData
 function Region:add_source(source) end

--- Creates a new Region instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return Region
function Region.new(parent, name) end

--- Creates a new Region instance
--- @return Region
function Region.new_simple() end

--- Return Region class object
--- @return Class
function Region.get_class() end

--- Trying to cast Object into Region
--- @param object Object to cast
--- @return Region
function Region.cast(object) end

--- 
--- 
--- @class RegionLayer : Instance
--- @field item StaticItem item to mine
RegionLayer = {}

--- Creates a new RegionLayer instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return RegionLayer
function RegionLayer.new(parent, name) end

--- Creates a new RegionLayer instance
--- @return RegionLayer
function RegionLayer.new_simple() end

--- Return RegionLayer class object
--- @return Class
function RegionLayer.get_class() end

--- Trying to cast Object into RegionLayer
--- @param object Object to cast
--- @return RegionLayer
function RegionLayer.cast(object) end

--- 
--- 
--- @class RegionMap : Instance
RegionMap = {}

---Delete all regions
 function RegionMap:reset() end

---Get Region by its position
---@param spos Vec2i position in RegionMap grid
---@return Region
 function RegionMap:get_region(spos) end

---Looking for existing Region with given sector position
---@param spos Vec2i position in RegionMap grid
---@return Region
 function RegionMap:find_region(spos) end

---Looking for SourceData near given location in world blocks
---@param wbpos Vec3i position in Dimension grid
---@return SourceData
 function RegionMap:find_source(wbpos) end

---Is region exists
---@param pos Vec2i region position
---@return boolean
 function RegionMap:has_region(pos) end

---Convert Block World position to RegionMap grid cell that contains this position
---@param bpos Vec3i
--- @return Vec2i RegionMap grid position
 function RegionMap.world_block_to_grid(bpos) end

--- Creates a new RegionMap instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return RegionMap
function RegionMap.new(parent, name) end

--- Creates a new RegionMap instance
--- @return RegionMap
function RegionMap.new_simple() end

--- Return RegionMap class object
--- @return Class
function RegionMap.get_class() end

--- Trying to cast Object into RegionMap
--- @param object Object to cast
--- @return RegionMap
function RegionMap.cast(object) end

--- 
--- 
--- @class ResourceAccessor : Accessor
--- @field inventory ResourceInventory undocumented
--- @field is_input boolean undocumented
--- @field is_output boolean undocumented
ResourceAccessor = {}

--- Creates a new ResourceAccessor instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return ResourceAccessor
function ResourceAccessor.new(parent, name) end

--- Return ResourceAccessor class object
--- @return Class
function ResourceAccessor.get_class() end

--- Trying to cast Object into ResourceAccessor
--- @param object Object to cast
--- @return ResourceAccessor
function ResourceAccessor.cast(object) end

--- 
--- 
--- @class ResourceInventory : SingleSlotInventory
--- @field item StaticItem undocumented
--- @field drain integer undocumented
ResourceInventory = {}

--- Creates a new ResourceInventory instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return ResourceInventory
function ResourceInventory.new(parent, name) end

--- Creates a new ResourceInventory instance
--- @return ResourceInventory
function ResourceInventory.new_simple() end

--- Return ResourceInventory class object
--- @return Class
function ResourceInventory.get_class() end

--- Trying to cast Object into ResourceInventory
--- @param object Object to cast
--- @return ResourceInventory
function ResourceInventory.cast(object) end

--- 
--- 
--- @class SelectCrafter : AbstractCrafter
SelectCrafter = {}

--- Creates a new SelectCrafter instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return SelectCrafter
function SelectCrafter.new(parent, name) end

--- Creates a new SelectCrafter instance
--- @return SelectCrafter
function SelectCrafter.new_simple() end

--- Return SelectCrafter class object
--- @return Class
function SelectCrafter.get_class() end

--- Trying to cast Object into SelectCrafter
--- @param object Object to cast
--- @return SelectCrafter
function SelectCrafter.cast(object) end

--- 
--- 
--- @class Setting : Prototype
--- @field restart boolean undocumented
--- @field set_action function undocumented
--- @field label string undocumented
--- @field type string undocumented
--- @field category string undocumented
--- @field bool_value boolean undocumented
--- @field bool_default_value boolean undocumented
--- @field string_value string undocumented
--- @field string_value string undocumented
--- @field string_options string[] undocumented
--- @field int_value integer undocumented
--- @field int_default_value integer undocumented
--- @field max_value integer undocumented
--- @field min_value integer undocumented
--- @field key_binding string undocumented
--- @field default_key string undocumented
Setting = {}

--- Register a new Setting static object
--- @param name string The name of the object
--- @return Setting
function Setting.reg(name) end

--- Searching for Setting in db
--- @param name string The name of the object
--- @return Setting
function Setting.find(name) end

--- Return Setting class object
--- @return Class
function Setting.get_class() end

--- Trying to cast Object into Setting
--- @param object Object to cast
--- @return Setting
function Setting.cast(object) end

--- 
--- 
--- @class SettingsCategory : Prototype
--- @field settings Setting[] undocumented
--- @field label string undocumented
--- @field file_name string undocumented
SettingsCategory = {}

--- Register a new SettingsCategory static object
--- @param name string The name of the object
--- @return SettingsCategory
function SettingsCategory.reg(name) end

--- Searching for SettingsCategory in db
--- @param name string The name of the object
--- @return SettingsCategory
function SettingsCategory.find(name) end

--- Return SettingsCategory class object
--- @return Class
function SettingsCategory.get_class() end

--- Trying to cast Object into SettingsCategory
--- @param object Object to cast
--- @return SettingsCategory
function SettingsCategory.cast(object) end

--- 
--- 
--- @class SimpleLayeringGenerator : LayeringGenerator
SimpleLayeringGenerator = {}

--- Register a new LayeringGenerator static object
--- @param name string The name of the object
--- @return LayeringGenerator
function LayeringGenerator.reg(name) end

--- Searching for LayeringGenerator in db
--- @param name string The name of the object
--- @return LayeringGenerator
function LayeringGenerator.find(name) end

--- Return LayeringGenerator class object
--- @return Class
function LayeringGenerator.get_class() end

--- Trying to cast Object into LayeringGenerator
--- @param object Object to cast
--- @return LayeringGenerator
function LayeringGenerator.cast(object) end

--- Register a new SimpleLayeringGenerator static object
--- @param name string The name of the object
--- @return SimpleLayeringGenerator
function SimpleLayeringGenerator.reg(name) end

--- Searching for SimpleLayeringGenerator in db
--- @param name string The name of the object
--- @return SimpleLayeringGenerator
function SimpleLayeringGenerator.find(name) end

--- Return SimpleLayeringGenerator class object
--- @return Class
function SimpleLayeringGenerator.get_class() end

--- Trying to cast Object into SimpleLayeringGenerator
--- @param object Object to cast
--- @return SimpleLayeringGenerator
function SimpleLayeringGenerator.cast(object) end

--- Register a new ChancedLayeringGenerator static object
--- @param name string The name of the object
--- @return ChancedLayeringGenerator
function ChancedLayeringGenerator.reg(name) end

--- Searching for ChancedLayeringGenerator in db
--- @param name string The name of the object
--- @return ChancedLayeringGenerator
function ChancedLayeringGenerator.find(name) end

--- Return ChancedLayeringGenerator class object
--- @return Class
function ChancedLayeringGenerator.get_class() end

--- Trying to cast Object into ChancedLayeringGenerator
--- @param object Object to cast
--- @return ChancedLayeringGenerator
function ChancedLayeringGenerator.cast(object) end

--- 
--- 
--- @class SingleSlotInventory : BaseInventory
--- @field capacity integer undocumented
SingleSlotInventory = {}

---@field item StaticItem
 function SingleSlotInventory:set_simple_filter(item) end

--- Creates a new SingleSlotInventory instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return SingleSlotInventory
function SingleSlotInventory.new(parent, name) end

--- Creates a new SingleSlotInventory instance
--- @return SingleSlotInventory
function SingleSlotInventory.new_simple() end

--- Return SingleSlotInventory class object
--- @return Class
function SingleSlotInventory.get_class() end

--- Trying to cast Object into SingleSlotInventory
--- @param object Object to cast
--- @return SingleSlotInventory
function SingleSlotInventory.cast(object) end

--- 
--- 
--- @class SourceData : Instance
--- @field position Vec2i source position in block coordinates
--- @field item StaticItem item to mine
--- @field initial_capacity integer initial source capacity (ore quantity)
--- @field current_ore integer current remaining ore quantity (clamped to 2% of initial)
--- @field active_miners integer number of miners currently extracting
--- @field infinite_ore boolean undocumented
SourceData = {}

--- Creates a new SourceData instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return SourceData
function SourceData.new(parent, name) end

--- Creates a new SourceData instance
--- @return SourceData
function SourceData.new_simple() end

--- Return SourceData class object
--- @return Class
function SourceData.get_class() end

--- Trying to cast Object into SourceData
--- @param object Object to cast
--- @return SourceData
function SourceData.cast(object) end

--- 
--- 
--- @class StaticBlock : StaticObject
--- @field logic Class undocumented
--- @field actor Class undocumented
--- @field selector Class undocumented
--- @field tesselator Tesselator undocumented
--- @field sub_blocks Vec3i[] undocumented
--- @field replace_tag string undocumented
--- @field color_side Vec3 undocumented
--- @field color_top Vec3 undocumented
--- @field tier integer undocumented
--- @field level integer undocumented
--- @field lua table undocumented
StaticBlock = {}

--- Register a new StaticBlock static object
--- @param name string The name of the object
--- @return StaticBlock
function StaticBlock.reg(name) end

--- Searching for StaticBlock in db
--- @param name string The name of the object
--- @return StaticBlock
function StaticBlock.find(name) end

--- Return StaticBlock class object
--- @return Class
function StaticBlock.get_class() end

--- Trying to cast Object into StaticBlock
--- @param object Object to cast
--- @return StaticBlock
function StaticBlock.cast(object) end

--- 
--- 
--- @class StaticCover : StaticObject
StaticCover = {}

--- Register a new StaticCover static object
--- @param name string The name of the object
--- @return StaticCover
function StaticCover.reg(name) end

--- Searching for StaticCover in db
--- @param name string The name of the object
--- @return StaticCover
function StaticCover.find(name) end

--- Return StaticCover class object
--- @return Class
function StaticCover.get_class() end

--- Trying to cast Object into StaticCover
--- @param object Object to cast
--- @return StaticCover
function StaticCover.cast(object) end

--- 
--- 
--- @class StaticItem : Prototype
--- @field image Texture Item image in UI
--- @field stack_size integer Size of item stack
--- @field tier integer Item unlock tier
--- @field unit_mul number multiplier for UI
--- @field mesh StaticMesh Mesh for item rendering in world
--- @field block StaticObject Buildable object pointer for this item
--- @field custom_data bool Is item instance contains CustomData
--- @field custom_data bool Is item instance contains CustomData
--- @field logic Class Class for item while in hand
--- @field lua table undocumented
--- @field tags string undocumented
--- @field category string In-game database category
--- @field label Loc undocumented
--- @field description_parts Loc[] undocumented
StaticItem = {}

--- Register a new StaticItem static object
--- @param name string The name of the object
--- @return StaticItem
function StaticItem.reg(name) end

--- Searching for StaticItem in db
--- @param name string The name of the object
--- @return StaticItem
function StaticItem.find(name) end

--- Return StaticItem class object
--- @return Class
function StaticItem.get_class() end

--- Trying to cast Object into StaticItem
--- @param object Object to cast
--- @return StaticItem
function StaticItem.cast(object) end

--- 
--- 
--- @class StaticObject : Prototype
--- @field item StaticItem undocumented
StaticObject = {}

--- Register a new StaticObject static object
--- @param name string The name of the object
--- @return StaticObject
function StaticObject.reg(name) end

--- Searching for StaticObject in db
--- @param name string The name of the object
--- @return StaticObject
function StaticObject.find(name) end

--- Return StaticObject class object
--- @return Class
function StaticObject.get_class() end

--- Trying to cast Object into StaticObject
--- @param object Object to cast
--- @return StaticObject
function StaticObject.cast(object) end

--- 
--- 
--- @class StaticProp : StaticObject
--- @field project_to_terrain_power number undocumented
--- @field additive_elevation number undocumented
--- @field cull_begin number undocumented
--- @field cull_end number undocumented
--- @field maximum_height number undocumented
--- @field minimum_height number undocumented
--- @field floating boolean undocumented
--- @field is_big boolean undocumented
--- @field mesh StaticMesh undocumented
--- @field no_collision boolean undocumented
--- @field is_emitting boolean undocumented
--- @field high_detail_shadow boolean undocumented
--- @field on_spawn function undocumented
--- @field hits_to_break integer Number of hits to break
--- @field break_effect Class undocumented
--- @field damage_effect Class undocumented
--- @field actor_class Class undocumented
StaticProp = {}

--- Register a new StaticProp static object
--- @param name string The name of the object
--- @return StaticProp
function StaticProp.reg(name) end

--- Searching for StaticProp in db
--- @param name string The name of the object
--- @return StaticProp
function StaticProp.find(name) end

--- Return StaticProp class object
--- @return Class
function StaticProp.get_class() end

--- Trying to cast Object into StaticProp
--- @param object Object to cast
--- @return StaticProp
function StaticProp.cast(object) end

--- Prototype asset that owns several PropListData records
--- 
--- @class StaticPropList : Prototype
--- @field data PropListData[] undocumented
StaticPropList = {}

--- Register a new StaticPropList static object
--- @param name string The name of the object
--- @return StaticPropList
function StaticPropList.reg(name) end

--- Searching for StaticPropList in db
--- @param name string The name of the object
--- @return StaticPropList
function StaticPropList.find(name) end

--- Return StaticPropList class object
--- @return Class
function StaticPropList.get_class() end

--- Trying to cast Object into StaticPropList
--- @param object Object to cast
--- @return StaticPropList
function StaticPropList.cast(object) end

--- Research base class
--- 
--- @class StaticResearch : Prototype
StaticResearch = {}

--- Register a new StaticResearch static object
--- @param name string The name of the object
--- @return StaticResearch
function StaticResearch.reg(name) end

--- Searching for StaticResearch in db
--- @param name string The name of the object
--- @return StaticResearch
function StaticResearch.find(name) end

--- Return StaticResearch class object
--- @return Class
function StaticResearch.get_class() end

--- Trying to cast Object into StaticResearch
--- @param object Object to cast
--- @return StaticResearch
function StaticResearch.cast(object) end

--- 
--- 
--- @class StaticResearchBonusInventory : StaticResearch
StaticResearchBonusInventory = {}

--- Register a new StaticResearchBonusInventory static object
--- @param name string The name of the object
--- @return StaticResearchBonusInventory
function StaticResearchBonusInventory.reg(name) end

--- Searching for StaticResearchBonusInventory in db
--- @param name string The name of the object
--- @return StaticResearchBonusInventory
function StaticResearchBonusInventory.find(name) end

--- Return StaticResearchBonusInventory class object
--- @return Class
function StaticResearchBonusInventory.get_class() end

--- Trying to cast Object into StaticResearchBonusInventory
--- @param object Object to cast
--- @return StaticResearchBonusInventory
function StaticResearchBonusInventory.cast(object) end

--- 
--- 
--- @class StaticResearchDecorationUnlock : StaticResearch
StaticResearchDecorationUnlock = {}

--- Register a new StaticResearchDecorationUnlock static object
--- @param name string The name of the object
--- @return StaticResearchDecorationUnlock
function StaticResearchDecorationUnlock.reg(name) end

--- Searching for StaticResearchDecorationUnlock in db
--- @param name string The name of the object
--- @return StaticResearchDecorationUnlock
function StaticResearchDecorationUnlock.find(name) end

--- Return StaticResearchDecorationUnlock class object
--- @return Class
function StaticResearchDecorationUnlock.get_class() end

--- Trying to cast Object into StaticResearchDecorationUnlock
--- @param object Object to cast
--- @return StaticResearchDecorationUnlock
function StaticResearchDecorationUnlock.cast(object) end

--- 
--- 
--- @class StaticResearchEfficiency : StaticResearch
StaticResearchEfficiency = {}

--- Register a new StaticResearchEfficiency static object
--- @param name string The name of the object
--- @return StaticResearchEfficiency
function StaticResearchEfficiency.reg(name) end

--- Searching for StaticResearchEfficiency in db
--- @param name string The name of the object
--- @return StaticResearchEfficiency
function StaticResearchEfficiency.find(name) end

--- Return StaticResearchEfficiency class object
--- @return Class
function StaticResearchEfficiency.get_class() end

--- Trying to cast Object into StaticResearchEfficiency
--- @param object Object to cast
--- @return StaticResearchEfficiency
function StaticResearchEfficiency.cast(object) end

--- 
--- 
--- @class StaticResearchModifier : StaticResearch
StaticResearchModifier = {}

--- Register a new StaticResearchModifier static object
--- @param name string The name of the object
--- @return StaticResearchModifier
function StaticResearchModifier.reg(name) end

--- Searching for StaticResearchModifier in db
--- @param name string The name of the object
--- @return StaticResearchModifier
function StaticResearchModifier.find(name) end

--- Return StaticResearchModifier class object
--- @return Class
function StaticResearchModifier.get_class() end

--- Trying to cast Object into StaticResearchModifier
--- @param object Object to cast
--- @return StaticResearchModifier
function StaticResearchModifier.cast(object) end

--- 
--- 
--- @class StaticResearchRecipe : StaticResearch
StaticResearchRecipe = {}

--- Register a new StaticResearchRecipe static object
--- @param name string The name of the object
--- @return StaticResearchRecipe
function StaticResearchRecipe.reg(name) end

--- Searching for StaticResearchRecipe in db
--- @param name string The name of the object
--- @return StaticResearchRecipe
function StaticResearchRecipe.find(name) end

--- Return StaticResearchRecipe class object
--- @return Class
function StaticResearchRecipe.get_class() end

--- Trying to cast Object into StaticResearchRecipe
--- @param object Object to cast
--- @return StaticResearchRecipe
function StaticResearchRecipe.cast(object) end

--- 
--- 
--- @class StaticResearchToolUnlock : StaticResearch
StaticResearchToolUnlock = {}

--- Register a new StaticResearchToolUnlock static object
--- @param name string The name of the object
--- @return StaticResearchToolUnlock
function StaticResearchToolUnlock.reg(name) end

--- Searching for StaticResearchToolUnlock in db
--- @param name string The name of the object
--- @return StaticResearchToolUnlock
function StaticResearchToolUnlock.find(name) end

--- Return StaticResearchToolUnlock class object
--- @return Class
function StaticResearchToolUnlock.get_class() end

--- Trying to cast Object into StaticResearchToolUnlock
--- @param object Object to cast
--- @return StaticResearchToolUnlock
function StaticResearchToolUnlock.cast(object) end

--- 
--- 
--- @class StaticStructure : Instance
--- @field generate function undocumented
--- @field size Vec2i undocumented
StaticStructure = {}

--- Register a new StaticStructure static object
--- @param name string The name of the object
--- @return StaticStructure
function StaticStructure.reg(name) end

--- Searching for StaticStructure in db
--- @param name string The name of the object
--- @return StaticStructure
function StaticStructure.find(name) end

--- Return StaticStructure class object
--- @return Class
function StaticStructure.get_class() end

--- Trying to cast Object into StaticStructure
--- @param object Object to cast
--- @return StaticStructure
function StaticStructure.cast(object) end

--- 
--- 
--- @class StaticTip : Prototype
StaticTip = {}

--- Register a new StaticTip static object
--- @param name string The name of the object
--- @return StaticTip
function StaticTip.reg(name) end

--- Searching for StaticTip in db
--- @param name string The name of the object
--- @return StaticTip
function StaticTip.find(name) end

--- Return StaticTip class object
--- @return Class
function StaticTip.get_class() end

--- Trying to cast Object into StaticTip
--- @param object Object to cast
--- @return StaticTip
function StaticTip.cast(object) end

--- 
--- 
--- @class StorageBlockLogic : BlockLogic
StorageBlockLogic = {}

--- Creates a new StorageBlockLogic instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return StorageBlockLogic
function StorageBlockLogic.new(parent, name) end

--- Creates a new StorageBlockLogic instance
--- @return StorageBlockLogic
function StorageBlockLogic.new_simple() end

--- Return StorageBlockLogic class object
--- @return Class
function StorageBlockLogic.get_class() end

--- Trying to cast Object into StorageBlockLogic
--- @param object Object to cast
--- @return StorageBlockLogic
function StorageBlockLogic.cast(object) end

--- 
--- 
--- @class SurfaceDefinition : Instance
--- @field region_map URegionMap undocumented
--- @field generator_name string undocumented
SurfaceDefinition = {}

--- Creates a new SurfaceDefinition instance
--- @param parent Object Object of parent
--- @param name string The name of the instance
--- @return SurfaceDefinition
function SurfaceDefinition.new(parent, name) end

--- Creates a new SurfaceDefinition instance
--- @return SurfaceDefinition
function SurfaceDefinition.new_simple() end

--- Return SurfaceDefinition class object
--- @return Class
function SurfaceDefinition.get_class() end

--- Trying to cast Object into SurfaceDefinition
--- @param object Object to cast
--- @return SurfaceDefinition
function SurfaceDefinition.cast(object) end

--- 
--- 
--- @class WorldGenerator : Prototype
WorldGenerator = {}

--- Register a new WorldGenerator static object
--- @param name string The name of the object
--- @return WorldGenerator
function WorldGenerator.reg(name) end

--- Searching for WorldGenerator in db
--- @param name string The name of the object
--- @return WorldGenerator
function WorldGenerator.find(name) end

--- Return WorldGenerator class object
--- @return Class
function WorldGenerator.get_class() end

--- Trying to cast Object into WorldGenerator
--- @param object Object to cast
--- @return WorldGenerator
function WorldGenerator.cast(object) end

