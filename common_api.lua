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

---
---
--- @class EngineData
engine = {}


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
