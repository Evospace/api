---@meta
--- LuaLS only: declarative `ui.Xxx { ... }` / `ui.Xxx "..."` callables (see Qr/Widget/ModWidget.cpp UMod*::LuaBuildFromTable).
--- Full method stubs live in generated api.lua; this file merges __call signatures with the table type from api.lua.
---@diagnostic disable: assign-type-mismatch

---@alias UiChild ui.Widget

--- ui.Window(title) or ui.Window { title = string, anchor_x?, anchor_y?, offset_x?, offset_y?, [1] = content? }
--- anchor_x: "left" | "center" | "right" (default "left")
--- anchor_y: "top" | "center" | "bottom" (default "top")
--- offset_x, offset_y: viewport pixels from anchor (default 0)
---@class ui.WindowSpec
---@field title string
---@field anchor_x? "left"|"center"|"right"
---@field anchor_y? "top"|"center"|"bottom"
---@field offset_x? number
---@field offset_y? number
---@field [integer] UiChild

--- ui.Border { padding = number?, [1] = child? }
---@class ui.BorderSpec
---@field padding? number
---@field [integer] UiChild

--- ui.Button(on_click) or ui.Button { on_click = function, [1] = child? }
---@class ui.ButtonSpec
---@field on_click fun()
---@field [integer] UiChild

--- ui.Text "text" or ui.Text { text = string, font_size = number? }
---@class ui.TextSpec
---@field text string
---@field font_size? number

--- ui.VBox { child1, ... } or ui.VBox()
---@class ui.VBoxSpec
---@field [integer] UiChild

--- ui.HBox { child1, ... } or ui.HBox()
---@class ui.HBoxSpec
---@field [integer] UiChild

---@overload fun(title: string): ui.Window
---@overload fun(spec: ui.WindowSpec): ui.Window
ui.Window = ui.Window

---@overload fun(spec: ui.BorderSpec): ui.Border
ui.Border = ui.Border

---@overload fun(on_click: fun()): ui.Button
---@overload fun(spec: ui.ButtonSpec): ui.Button
ui.Button = ui.Button

---@overload fun(text: string): ui.Text
---@overload fun(spec: ui.TextSpec): ui.Text
ui.Text = ui.Text

---@overload fun(): ui.VBox
---@overload fun(spec: ui.VBoxSpec): ui.VBox
ui.VBox = ui.VBox

---@overload fun(): ui.HBox
---@overload fun(spec: ui.HBoxSpec): ui.HBox
ui.HBox = ui.HBox
