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

-- end of common --

--- @class AbstractCrafter : BlockLogic
--- @field recipes RecipeDictionary
--- @field load_independent boolean
--- @field stable_supply boolean
--- @field input_gathered boolean
--- @field switch_on boolean
--- @field ticks_passed integer
--- @field real_ticks_passed integer
--- @field total_production integer
--- @field speed integer
--- @field energy_input_inventory ResourceInventory
--- @field energy_output_inventory ResourceInventory
--- @field crafter_input_container ResourceInventory
--- @field crafter_output_container ResourceInventory
AbstractCrafter = {}

--- Creates a new AbstractCrafter instance
--- @param parent Object Object of parent
--- @param new_name string The name of the instance
--- @return AbstractCrafter
function AbstractCrafter.new(parent, new_name) end

--- Return AbstractCrafter class object
--- @return Class
function AbstractCrafter.get_class() end

--- Trying to cast Object into AbstractCrafter
--- @param object Object to cast
--- @return AbstractCrafter
function AbstractCrafter.cast(object) end

--- @class Accessor : Instance
--- @field side Vec3i
--- @field pos Vec3i
--- @field cover StaticCover
Accessor = {}

--- Creates a new Accessor instance
--- @param parent Object Object of parent
--- @param new_name string The name of the instance
--- @return Accessor
function Accessor.new(parent, new_name) end

--- Return Accessor class object
--- @return Class
function Accessor.get_class() end

--- Trying to cast Object into Accessor
--- @param object Object to cast
--- @return Accessor
function Accessor.cast(object) end

--- @class AutosizeInventory : Inventory
AutosizeInventory = {}

--- Creates a new AutosizeInventory instance
--- @param parent Object Object of parent
--- @param new_name string The name of the instance
--- @return AutosizeInventory
function AutosizeInventory.new(parent, new_name) end

--- Return AutosizeInventory class object
--- @return Class
function AutosizeInventory.get_class() end

--- Trying to cast Object into AutosizeInventory
--- @param object Object to cast
--- @return AutosizeInventory
function AutosizeInventory.cast(object) end

--- @class BaseInventory : InventoryAccess
BaseInventory = {}

--- Creates a new BaseInventory instance
--- @param parent Object Object of parent
--- @param new_name string The name of the instance
--- @return BaseInventory
function BaseInventory.new(parent, new_name) end

--- Return BaseInventory class object
--- @return Class
function BaseInventory.get_class() end

--- Trying to cast Object into BaseInventory
--- @param object Object to cast
--- @return BaseInventory
function BaseInventory.cast(object) end

--- @class BlockLogic : Instance
--- @field static_block StaticBlock comment
BlockLogic = {}

--- Creates a new BlockLogic instance
--- @param parent Object Object of parent
--- @param new_name string The name of the instance
--- @return BlockLogic
function BlockLogic.new(parent, new_name) end

--- Return BlockLogic class object
--- @return Class
function BlockLogic.get_class() end

--- Trying to cast Object into BlockLogic
--- @param object Object to cast
--- @return BlockLogic
function BlockLogic.cast(object) end

--- @class ConductorBlockLogic : StorageBlockLogic
--- @field side_cover StaticCover
--- @field center_cover StaticCover
--- @field resistance integer
--- @field conductor_channel integer
ConductorBlockLogic = {}

---Add side wire
---@param acc ResourceAccessor
function ConductorBlockLogic:add_wire(acc) end

--- Creates a new ConductorBlockLogic instance
--- @param parent Object Object of parent
--- @param new_name string The name of the instance
--- @return ConductorBlockLogic
function ConductorBlockLogic.new(parent, new_name) end

--- Return ConductorBlockLogic class object
--- @return Class
function ConductorBlockLogic.get_class() end

--- Trying to cast Object into ConductorBlockLogic
--- @param object Object to cast
--- @return ConductorBlockLogic
function ConductorBlockLogic.cast(object) end

--- @class DB : Object
DB = {}

---Register Prototype in DB
---@param proto Prototype Prototype to register
function DB:reg(proto) end

---Register mod table
---@param table table Mod table
function DB:mod(table) end

--- @class Dimension : Actor
--- @field settings GameSessionData
Dimension = {}

--- @class EventSystem : Object
EventSystem = {}

--- Get global instance of EventSystem
--- @return EventSystem
function EventSystem.get_instance() end

--- Subscribe
--- @param event integer Event id
--- @param action function Triggering action
--- @return integer Subscription id
function EventSystem:sub(event, action) end

--- Unsubscribe
--- @param event integer Event id
--- @param id integer Subscription id
function EventSystem:unsub(event, id) end

--- @class ExtractionData : Object
--- @field item StaticItem
--- @field speed number
ExtractionData = {}

--- Create new instance of ExtractionData
--- @return ExtractionData
function ExtractionData.new() end

--- @class GameSessionData : Object
--- @field infinite_ore boolean
GameSessionData = {}

--- @class Inventory : BaseInventory
Inventory = {}

--- Creates a new Inventory instance
--- @param parent Object Object of parent
--- @param new_name string The name of the instance
--- @return Inventory
function Inventory.new(parent, new_name) end

--- Return Inventory class object
--- @return Class
function Inventory.get_class() end

--- Trying to cast Object into Inventory
--- @param object Object to cast
--- @return Inventory
function Inventory.cast(object) end

--- @class InventoryAccess : InventoryReader
InventoryAccess = {}

--- Creates a new InventoryAccess instance
--- @param parent Object Object of parent
--- @param new_name string The name of the instance
--- @return InventoryAccess
function InventoryAccess.new(parent, new_name) end

--- Return InventoryAccess class object
--- @return Class
function InventoryAccess.get_class() end

--- Trying to cast Object into InventoryAccess
--- @param object Object to cast
--- @return InventoryAccess
function InventoryAccess.cast(object) end

--- @class InventoryReader : Instance
InventoryReader = {}

--- Creates a new InventoryReader instance
--- @param parent Object Object of parent
--- @param new_name string The name of the instance
--- @return InventoryReader
function InventoryReader.new(parent, new_name) end

--- Return InventoryReader class object
--- @return Class
function InventoryReader.get_class() end

--- Trying to cast Object into InventoryReader
--- @param object Object to cast
--- @return InventoryReader
function InventoryReader.cast(object) end

--- @class MapStructure : Object
--- @field offset Vec2i
--- @field structure StaticStructure
MapStructure = {}

--- Create new instance of MapStructure
--- @return MapStructure
function MapStructure.new() end

--- @class RecipeDictionary : Prototype
RecipeDictionary = {}

--- Creates a new RecipeDictionary static object
--- @param new_name string The name of the object
--- @return RecipeDictionary
function RecipeDictionary.new(new_name) end

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

--- @class ResourceAccessor : Accessor
--- @field inventory ResourceInventory
--- @field is_input boolean
--- @field is_output boolean
ResourceAccessor = {}

--- Creates a new ResourceAccessor instance
--- @param parent Object Object of parent
--- @param new_name string The name of the instance
--- @return ResourceAccessor
function ResourceAccessor.new(parent, new_name) end

--- Return ResourceAccessor class object
--- @return Class
function ResourceAccessor.get_class() end

--- Trying to cast Object into ResourceAccessor
--- @param object Object to cast
--- @return ResourceAccessor
function ResourceAccessor.cast(object) end

--- @class ResourceInventory : SingleSlotInventory
--- @field drain integer
ResourceInventory = {}

--- Creates a new ResourceInventory instance
--- @param parent Object Object of parent
--- @param new_name string The name of the instance
--- @return ResourceInventory
function ResourceInventory.new(parent, new_name) end

--- Return ResourceInventory class object
--- @return Class
function ResourceInventory.get_class() end

--- Trying to cast Object into ResourceInventory
--- @param object Object to cast
--- @return ResourceInventory
function ResourceInventory.cast(object) end

--- @class SingleSlotInventory : BaseInventory
--- @field capacity integer
SingleSlotInventory = {}

--- Creates a new SingleSlotInventory instance
--- @param parent Object Object of parent
--- @param new_name string The name of the instance
--- @return SingleSlotInventory
function SingleSlotInventory.new(parent, new_name) end

--- Return SingleSlotInventory class object
--- @return Class
function SingleSlotInventory.get_class() end

--- Trying to cast Object into SingleSlotInventory
--- @param object Object to cast
--- @return SingleSlotInventory
function SingleSlotInventory.cast(object) end

--- @class StaticAttach : StaticObject
StaticAttach = {}

--- Creates a new StaticAttach static object
--- @param new_name string The name of the object
--- @return StaticAttach
function StaticAttach.new(new_name) end

--- Searching for StaticAttach in db
--- @param name string The name of the object
--- @return StaticAttach
function StaticAttach.find(name) end

--- Return StaticAttach class object
--- @return Class
function StaticAttach.get_class() end

--- Trying to cast Object into StaticAttach
--- @param object Object to cast
--- @return StaticAttach
function StaticAttach.cast(object) end

--- @class StaticBlock : StaticObject
--- @field logic Class
--- @field actor Class
--- @field selector Class
--- @field tesselator Tesselator
--- @field color_side Vector
--- @field color_top Vector
--- @field tier integer
--- @field level integer
--- @field lua table
StaticBlock = {}

--- Creates a new StaticBlock static object
--- @param new_name string The name of the object
--- @return StaticBlock
function StaticBlock.new(new_name) end

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

--- @class StaticCover : StaticAttach
StaticCover = {}

--- Creates a new StaticCover static object
--- @param new_name string The name of the object
--- @return StaticCover
function StaticCover.new(new_name) end

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

--- @class StaticItem : Prototype
--- @field image Texture
--- @field stack_size integer
--- @field unit_mul number
--- @field mesh StaticMesh
--- @field craftable boolean
StaticItem = {}

--- Creates a new StaticItem static object
--- @param new_name string The name of the object
--- @return StaticItem
function StaticItem.new(new_name) end

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

--- @class StaticObject : Prototype
--- @field item StaticItem
StaticObject = {}

--- Creates a new StaticObject static object
--- @param new_name string The name of the object
--- @return StaticObject
function StaticObject.new(new_name) end

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

--- @class StaticStructure : Prototype
--- @field generate function|nil
--- @field size Vec2i
StaticStructure = {}

--- Creates a new StaticStructure static object
--- @param new_name string The name of the object
--- @return StaticStructure
function StaticStructure.new(new_name) end

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

--- @class StorageBlockLogic : BlockLogic
StorageBlockLogic = {}

--- Creates a new StorageBlockLogic instance
--- @param parent Object Object of parent
--- @param new_name string The name of the instance
--- @return StorageBlockLogic
function StorageBlockLogic.new(parent, new_name) end

--- Return StorageBlockLogic class object
--- @return Class
function StorageBlockLogic.get_class() end

--- Trying to cast Object into StorageBlockLogic
--- @param object Object to cast
--- @return StorageBlockLogic
function StorageBlockLogic.cast(object) end

