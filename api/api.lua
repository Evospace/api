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
--- @param new_name string The name of the instance
--- @return AbstractCrafter
function AbstractCrafter.new(parent, new_name) end

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

--- 
--- 
--- @class AutoCrafter : SelectCrafter
AutoCrafter = {}

--- Creates a new AutoCrafter instance
--- @param parent Object Object of parent
--- @param new_name string The name of the instance
--- @return AutoCrafter
function AutoCrafter.new(parent, new_name) end

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
--- @param new_name string The name of the instance
--- @return AutosizeInventory
function AutosizeInventory.new(parent, new_name) end

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
--- @param new_name string The name of the instance
--- @return BaseInventory
function BaseInventory.new(parent, new_name) end

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
--- @class BlockLogic : Instance
--- @field static_block StaticBlock comment
BlockLogic = {}

--- Register Accessor
--- @param accessor Accessor Accessor instance to register
function BlockLogic:reg(accessor) end

--- Creates a new BlockLogic instance
--- @param parent Object Object of parent
--- @param new_name string The name of the instance
--- @return BlockLogic
function BlockLogic.new(parent, new_name) end

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
--- @class ComputerBlockLogic : BlockLogic
--- @field energy_inventory ResourceInventory undocumented
--- @field energy_input ResourceAccessor undocumented
ComputerBlockLogic = {}

--- Creates a new ComputerBlockLogic instance
--- @param parent Object Object of parent
--- @param new_name string The name of the instance
--- @return ComputerBlockLogic
function ComputerBlockLogic.new(parent, new_name) end

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
--- @field resistance integer mOhm
--- @field voltage integer Volt
--- @field channel string undocumented
--- @field conductor_channel integer undocumented
ConductorBlockLogic = {}

---Add side wire
---@param acc ResourceAccessor
function ConductorBlockLogic:add_wire(acc) end

--- Creates a new ConductorBlockLogic instance
--- @param parent Object Object of parent
--- @param new_name string The name of the instance
--- @return ConductorBlockLogic
function ConductorBlockLogic.new(parent, new_name) end

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

--- Comment to class
--- 
--- @class DB : Instance
DB = {}

---Register Prototype in DB
---@param proto Prototype Prototype to register
function DB:reg(proto) end

---Register object with class "class" and name "name" from table, filling all other properties from from table too
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
--- @param new_name string The name of the instance
--- @return DB
function DB.new(parent, new_name) end

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
--- @field settings GameSessionData undocumented
Dimension = {}

--- 
--- 
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

--- Emmit
--- @param event integer Event id
--- @param table Context table
function EventSystem:emmit(event, table) end

--- 
--- 
--- @class ExtractionData : Object
--- @field item StaticItem undocumented
--- @field prop StaticProp undocumented
--- @field speed integer undocumented
--- @field initial_capacity integer undocumented
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
--- @param new_name string The name of the instance
--- @return FluidContainerBlockLogic
function FluidContainerBlockLogic.new(parent, new_name) end

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
--- @class GameSessionData : Object
--- @field infinite_ore boolean undocumented
GameSessionData = {}

--- 
--- 
--- @class Inventory : BaseInventory
Inventory = {}

--- Creates a new Inventory instance
--- @param parent Object Object of parent
--- @param new_name string The name of the instance
--- @return Inventory
function Inventory.new(parent, new_name) end

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
--- @field is_can_have_zero_slot boolean undocumented
InventoryAccess = {}

--- Add item with count to InventoryAccess
--- @param item StaticItem
--- @param count integer
function InventoryAccess:add(item, count) end

--- Creates a new InventoryAccess instance
--- @param parent Object Object of parent
--- @param new_name string The name of the instance
--- @return InventoryAccess
function InventoryAccess.new(parent, new_name) end

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
--- @param new_name string The name of the instance
--- @return InventoryBlackFilter
function InventoryBlackFilter.new(parent, new_name) end

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
function InventoryContainer:get_access(index) end

---@field inventory InventoryAccess
function InventoryContainer:bind(inventory) end

--- Creates a new InventoryContainer instance
--- @param parent Object Object of parent
--- @param new_name string The name of the instance
--- @return InventoryContainer
function InventoryContainer.new(parent, new_name) end

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
--- @param new_name string The name of the instance
--- @return InventoryFilter
function InventoryFilter.new(parent, new_name) end

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
--- @param new_name string The name of the instance
--- @return InventoryInventoryFilter
function InventoryInventoryFilter.new(parent, new_name) end

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
--- @param new_name string The name of the instance
--- @return InventoryReader
function InventoryReader.new(parent, new_name) end

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
--- @param new_name string The name of the instance
--- @return InventoryWhiteFilter
function InventoryWhiteFilter.new(parent, new_name) end

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
--- @class MapStructure : Object
--- @field offset Vec2i undocumented
--- @field structure StaticStructure undocumented
MapStructure = {}

--- Create new instance of MapStructure
--- @return MapStructure
function MapStructure.new() end

--- 
--- 
--- @class PropListData : Struct
--- @field chance number undocumented
--- @field props StaticProp[] undocumented
PropListData = {}

--- Database record
--- 
--- @class Prototype : Object
--- @field name string Object name
Prototype = {}

--- Creates a new type instance
--- @param parent Object Object of parent
--- @param new_name string The name of the instance
--- @return type
function type.new(parent, new_name) end

--- Return type class object
--- @return Class
function type.get_class() end

--- Trying to cast Object into type
--- @param object Object to cast
--- @return type
function type.cast(object) end

--- Creates a new type instance
--- @param parent Object Object of parent
--- @param new_name string The name of the instance
--- @return type
function type.new(parent, new_name) end

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

--- Creates a new type static object
--- @param new_name string The name of the object
--- @return type
function type.new(new_name) end

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
--- @class RecipeDictionary : Prototype
--- @field start_tier integer undocumented
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

--- 
--- 
--- @class Region : Instance
Region = {}

---Add source to this Region
---@param spos SourceData
function Region:add_source(spos) end

--- Creates a new Region instance
--- @param parent Object Object of parent
--- @param new_name string The name of the instance
--- @return Region
function Region.new(parent, new_name) end

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
--- @class RegionMap : Instance
RegionMap = {}

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

---Convert Block World position to RegionMap grid cell that contains this position
---@param bpos Vec3i
--- @return Vec2i RegionMap grid position
function RegionMap.world_block_to_grid(bpos) end

--- Creates a new RegionMap instance
--- @param parent Object Object of parent
--- @param new_name string The name of the instance
--- @return RegionMap
function RegionMap.new(parent, new_name) end

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

--- 
--- 
--- @class ResourceInventory : SingleSlotInventory
--- @field item StaticItem undocumented
--- @field drain integer undocumented
ResourceInventory = {}

--- Creates a new ResourceInventory instance
--- @param parent Object Object of parent
--- @param new_name string The name of the instance
--- @return ResourceInventory
function ResourceInventory.new(parent, new_name) end

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
--- @class SelectCrafter : AutoCrafter
SelectCrafter = {}

--- Creates a new SelectCrafter instance
--- @param parent Object Object of parent
--- @param new_name string The name of the instance
--- @return SelectCrafter
function SelectCrafter.new(parent, new_name) end

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
--- @class SingleSlotInventory : BaseInventory
--- @field capacity integer undocumented
SingleSlotInventory = {}

--- Creates a new SingleSlotInventory instance
--- @param parent Object Object of parent
--- @param new_name string The name of the instance
--- @return SingleSlotInventory
function SingleSlotInventory.new(parent, new_name) end

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
SourceData = {}

--- Creates a new SourceData instance
--- @param parent Object Object of parent
--- @param new_name string The name of the instance
--- @return SourceData
function SourceData.new(parent, new_name) end

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
--- @field replace_tag string undocumented
--- @field color_side Vector undocumented
--- @field color_top Vector undocumented
--- @field tier integer undocumented
--- @field level integer undocumented
--- @field lua table undocumented
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

--- 
--- 
--- @class StaticCover : StaticObject
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

--- 
--- 
--- @class StaticItem : Prototype
--- @field image Texture undocumented
--- @field stack_size integer undocumented
--- @field unit_mul number undocumented
--- @field mesh StaticMesh undocumented
--- @field object StaticObject undocumented
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

--- 
--- 
--- @class StaticObject : Prototype
--- @field item StaticItem undocumented
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
--- @field mesh StaticItem undocumented
--- @field no_collision boolean undocumented
--- @field is_emitting boolean undocumented
--- @field break_chance integer Break chance in percents
StaticProp = {}

--- Creates a new StaticProp static object
--- @param new_name string The name of the object
--- @return StaticProp
function StaticProp.new(new_name) end

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

--- 
--- 
--- @class StaticPropList : Prototype
--- @field data PropListData[] undocumented
StaticPropList = {}

--- Creates a new StaticPropList static object
--- @param new_name string The name of the object
--- @return StaticPropList
function StaticPropList.new(new_name) end

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

--- 
--- 
--- @class StaticStructure : Instance
--- @field generate function undocumented
--- @field size Vec2i undocumented
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

--- 
--- 
--- @class StorageBlockLogic : BlockLogic
StorageBlockLogic = {}

--- Creates a new StorageBlockLogic instance
--- @param parent Object Object of parent
--- @param new_name string The name of the instance
--- @return StorageBlockLogic
function StorageBlockLogic.new(parent, new_name) end

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

