Mod widgets (Lua UI)
====================

Mod widgets are UMG-based UI elements registered on the main game Lua state. Use them to build simple in-game panels without Blueprints. Widgets are owned by the world’s **first player controller**; creation must happen while that controller exists (typical game / client context).

Layout overview
---------------

* Build a tree of **ModWidget** subclasses (boxes, text, border, button).
* Put the subtree inside a **ModWindow** (``content`` / first array slot in the table DSL).
* Call **:show()** on the window to **AddToViewport** (default Z order 0). Call **:close()** to remove it from the parent.

Each child may have **only one parent**. If you hit errors about a widget already having a slot, create a new instance or remove it from the old parent first.

Two construction styles
------------------------

**1. Declarative (callable globals).** Types ``ModWindow``, ``ModButton``, ``ModBorder``, ``ModTextBlock``, ``ModVerticalBox``, and ``ModHorizontalBox`` are callable. They accept either a string / function / table as documented on each type below.

**2. Imperative.** Use ``Type.new(...)`` and then set fields or call **:add()** on containers. This mirrors the generated stubs in **api.lua**.

Window placement (``ModWindow`` table form)
--------------------------------------------

When you build a window with a table, optional fields control canvas placement (viewport pixels from the chosen anchor):

* ``anchor_x``: ``"left"`` (default), ``"center"``, or ``"right"``
* ``anchor_y``: ``"top"`` (default), ``"center"``, or ``"bottom"``
* ``offset_x``, ``offset_y``: numeric offsets from that anchor

Example (declarative)
---------------------

.. code-block:: lua

   local w = ModWindow {
     title = "My mod",
     anchor_x = "center",
     anchor_y = "center",
     ModVerticalBox {
       ModTextBlock { text = "Hello", font_size = 18 },
       ModButton {
         on_click = function()
           print("clicked")
         end,
         ModTextBlock "OK",
       },
     },
   }
   w:show()

Reference
---------

.. lua:autoobject:: ModWidget
   :members:

.. lua:autoobject:: ModWindow
   :members:

.. lua:autoobject:: ModVerticalBox
   :members:

.. lua:autoobject:: ModHorizontalBox
   :members:

.. lua:autoobject:: ModBorder
   :members:

.. lua:autoobject:: ModButton
   :members:

.. lua:autoobject:: ModTextBlock
   :members:
