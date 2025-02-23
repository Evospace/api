--- @class Prototype : Object
Prototype = {}

--- @class Instance : Object
Instance = {}

--- @class Vec3i
--- @field zero Vec3i = (0, 0, 0)
--- @field one Vec3i = (1, 1, 1)
--- @field left Vec3i = (0, 1, 0)
--- @field right Vec3i = (0, -1, 0)
--- @field up Vec3i = (0, 0, 1)
--- @field down Vec3i = (0, 0, -1)
--- @field front Vec3i = (1, 0, 0)
--- @field back Vec3i = (-1, 0, 0)
Vec3i = {}

--- @class Vec3
--- @field zero Vec3 = (0, 0, 0)
--- @field one Vec3 = (1, 1, 1)
--- @field left Vec3 = (0, 1, 0)
--- @field right Vec3 = (0, -1, 0)
--- @field up Vec3 = (0, 0, 1)
--- @field down Vec3 = (0, 0, -1)
--- @field front Vec3 = (1, 0, 0)
--- @field back Vec3 = (-1, 0, 0)
Vec3 = {}

--- @param x integer 
--- @param y integer 
--- @param z integer 
--- @return Vec3i
function Vec3i.new(x, y, z) end

--- @class Vec2i
--- @field zero Vec2i
--- @field one Vec2i
Vec2i = {}

--- @param x integer 
--- @param y integer 
--- @return Vec2i
function Vec2i.new(x, y) end

--- @class Vector
Vector = {}

--- @class Class
Class = {}

--- @class Texture
Texture = {}

--- @class Material
Material = {}

--- @param path string Path to the object
--- @return Material
function Material.load(path) end

--- @class Object
Object = {}

--- @class DB
db = {}

--- @class RegionMap
regions = {}

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