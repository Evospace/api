--- @class Prototype : Object
Prototype = {}

--- @class Instance : Object
Instance = {}

--- @class Vec3i
--- @field zero Vec3i
--- @field one Vec3i
--- @field left Vec3i
--- @field right Vec3i
--- @field up Vec3i
--- @field down Vec3i
--- @field front Vec3i
--- @field back Vec3i
Vec3i = {}

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

regions = {}