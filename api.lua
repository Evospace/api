--- @class Prototype
Prototype = {}

--- @class Instance
Instance = {}

-- end of common --

--- @class StaticItem : Prototype
StaticItem = {}

--- Creates a new StaticItem static object
--- @param new_name string The name of the instance
--- @return StaticItem
function StaticItem.new(new_name) end

--- Searching for StaticItem in db
--- @param name string The name of the object
--- @return StaticItem
function StaticItem.find(name) end

--- @class BaseInventory : InventoryAccess
BaseInventory = {}

--- Creates a new BaseInventory instance
--- @param parent Instance Instance of parent
--- @param new_name string The name of the instance
--- @return BaseInventory
function BaseInventory.new(parent, new_name) end

--- @class InventoryAccess : InventoryReader
InventoryAccess = {}

--- Creates a new InventoryAccess instance
--- @param parent Instance Instance of parent
--- @param new_name string The name of the instance
--- @return InventoryAccess
function InventoryAccess.new(parent, new_name) end

--- @class ResourceAccessor : Accessor
ResourceAccessor = {}

--- Creates a new ResourceAccessor instance
--- @param parent Instance Instance of parent
--- @param new_name string The name of the instance
--- @return ResourceAccessor
function ResourceAccessor.new(parent, new_name) end

--- @class StaticObject : Prototype
StaticObject = {}

--- Creates a new StaticObject static object
--- @param new_name string The name of the instance
--- @return StaticObject
function StaticObject.new(new_name) end

--- Searching for StaticObject in db
--- @param name string The name of the object
--- @return StaticObject
function StaticObject.find(name) end

--- @class Inventory : BaseInventory
Inventory = {}

--- Creates a new Inventory instance
--- @param parent Instance Instance of parent
--- @param new_name string The name of the instance
--- @return Inventory
function Inventory.new(parent, new_name) end

--- @class AutosizeInventory : Inventory
AutosizeInventory = {}

--- Creates a new AutosizeInventory instance
--- @param parent Instance Instance of parent
--- @param new_name string The name of the instance
--- @return AutosizeInventory
function AutosizeInventory.new(parent, new_name) end

--- @class StaticBlock : StaticObject
StaticBlock = {}

--- @class InventoryReader : Instance
InventoryReader = {}

--- Creates a new InventoryReader instance
--- @param parent Instance Instance of parent
--- @param new_name string The name of the instance
--- @return InventoryReader
function InventoryReader.new(parent, new_name) end

--- @class StaticAttach : StaticObject
StaticAttach = {}

--- Creates a new StaticAttach static object
--- @param new_name string The name of the instance
--- @return StaticAttach
function StaticAttach.new(new_name) end

--- Searching for StaticAttach in db
--- @param name string The name of the object
--- @return StaticAttach
function StaticAttach.find(name) end

--- Creates a new StaticCover static object
--- @param new_name string The name of the instance
--- @return StaticCover
function StaticCover.new(new_name) end

--- Searching for StaticCover in db
--- @param name string The name of the object
--- @return StaticCover
function StaticCover.find(name) end

--- @class StaticCover : StaticAttach
StaticCover = {}

--- Creates a new StaticAttach static object
--- @param new_name string The name of the instance
--- @return StaticAttach
function StaticAttach.new(new_name) end

--- Searching for StaticAttach in db
--- @param name string The name of the object
--- @return StaticAttach
function StaticAttach.find(name) end

--- Creates a new StaticCover static object
--- @param new_name string The name of the instance
--- @return StaticCover
function StaticCover.new(new_name) end

--- Searching for StaticCover in db
--- @param name string The name of the object
--- @return StaticCover
function StaticCover.find(name) end

--- @class Accessor : Instance
Accessor = {}

--- Creates a new Accessor instance
--- @param parent Instance Instance of parent
--- @param new_name string The name of the instance
--- @return Accessor
function Accessor.new(parent, new_name) end

