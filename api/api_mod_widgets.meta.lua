---@meta
--- LuaLS only: declarative `ModXxx { ... }` / `ModXxx "..."` callables (see ModWidget.cpp UMod*::LuaBuildFromTable).
--- Full method stubs live in generated api.lua; this file merges __call signatures with the table type from api.lua.
---@diagnostic disable: assign-type-mismatch

---@alias ModUiChild ModWidget

--- ModWindow(title) or ModWindow { title = string, anchor_x?, anchor_y?, offset_x?, offset_y?, [1] = content? }
--- anchor_x: "left" | "center" | "right" (default "left")
--- anchor_y: "top" | "center" | "bottom" (default "top")
--- offset_x, offset_y: viewport pixels from anchor (default 0)
---@class ModWindowSpec
---@field title string
---@field anchor_x? "left"|"center"|"right"
---@field anchor_y? "top"|"center"|"bottom"
---@field offset_x? number
---@field offset_y? number
---@field [integer] ModUiChild

--- ModBorder { padding = number?, [1] = child? }
---@class ModBorderSpec
---@field padding? number
---@field [integer] ModUiChild

--- ModButton(on_click) or ModButton { on_click = function, [1] = child? }
---@class ModButtonSpec
---@field on_click fun()
---@field [integer] ModUiChild

--- ModTextBlock "text" or ModTextBlock { text = string, font_size = number? }
---@class ModTextBlockSpec
---@field text string
---@field font_size? number

--- ModVerticalBox { child1, ... } or ModVerticalBox()
---@class ModVerticalBoxSpec
---@field [integer] ModUiChild

--- ModHorizontalBox { child1, ... } or ModHorizontalBox()
---@class ModHorizontalBoxSpec
---@field [integer] ModUiChild

---@overload fun(title: string): ModWindow
---@overload fun(spec: ModWindowSpec): ModWindow
ModWindow = ModWindow

---@overload fun(spec: ModBorderSpec): ModBorder
ModBorder = ModBorder

---@overload fun(on_click: fun()): ModButton
---@overload fun(spec: ModButtonSpec): ModButton
ModButton = ModButton

---@overload fun(text: string): ModTextBlock
---@overload fun(spec: ModTextBlockSpec): ModTextBlock
ModTextBlock = ModTextBlock

---@overload fun(): ModVerticalBox
---@overload fun(spec: ModVerticalBoxSpec): ModVerticalBox
ModVerticalBox = ModVerticalBox

---@overload fun(): ModHorizontalBox
---@overload fun(spec: ModHorizontalBoxSpec): ModHorizontalBox
ModHorizontalBox = ModHorizontalBox
