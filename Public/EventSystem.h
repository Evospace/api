// Copyright (c) 2017 - 2022, Samsonov Andrey. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ThirdParty/luabridge/LuaBridge.h"
#include <optional>
#include <string>

namespace evo {
class LuaState;
}

namespace defines {

// Single source of truth for gameplay events exposed to Lua.
// To add an event, add one line here: the `Event` enum, `ToString`, and the
// `defines.events` Lua namespace (see EventSystem::lua_reg) are all generated
// from this list, so they can never drift out of sync.
//
// Context tables passed to handlers (per event):
//   on_player_mined_item  { item, count }
//   on_built_block        { block, position }
//   on_research_finished  { research }
//   on_quest_activated    { quest }
//   on_surface_day_phase  { anchor }  -- string: "dawn" / "sunset"
#define EVO_EVENT_LIST(X)   \
  X(on_player_mined_item)   \
  X(on_built_block)         \
  X(on_player_spawn)        \
  X(on_tick)                \
  X(on_region_spawn)        \
  X(on_player_at_sector)    \
  X(on_entity_died)         \
  X(on_entity_damaged)      \
  X(on_entity_spawn)        \
  X(on_surface_day_phase)   \
  X(on_research_finished)   \
  X(on_quest_activated)

enum class Event {
#define EVO_EVENT_ENUM(name) name,
  EVO_EVENT_LIST(EVO_EVENT_ENUM)
#undef EVO_EVENT_ENUM
  Count
};

inline const char *ToString(Event event) {
  switch (event) {
#define EVO_EVENT_TOSTRING(name) \
  case Event::name:              \
    return #name;
    EVO_EVENT_LIST(EVO_EVENT_TOSTRING)
#undef EVO_EVENT_TOSTRING
  default:
    return "unknown_event";
  }
}

inline bool IsValidEvent(Event event) {
  return event >= static_cast<Event>(0) && event < Event::Count;
}

inline int EventIndex(Event event) {
  return static_cast<int>(event);
}

constexpr int EventCount = static_cast<int>(Event::Count);
} // namespace defines

class EventSystem {
  public:
  using NativeHandler = TFunction<void(const luabridge::LuaRef &)>;
  using HandlerID = int32;
  using SubscriberGroup = uint32;

  static constexpr SubscriberGroup NoGroup = 0;

  struct FHandlerSlot {
    HandlerID Id = 0;
    SubscriberGroup Group = NoGroup;
    std::optional<luabridge::LuaRef> LuaFunc;
    NativeHandler NativeFunc;
  };

  static EventSystem &get();
  void lua_cleanup();
  lua_State *L() const;

  HandlerID Sub(int event, luabridge::LuaRef func, SubscriberGroup group = NoGroup);

  HandlerID SubNative(defines::Event event, NativeHandler handler, SubscriberGroup group = NoGroup);

  void Unsub(int event, HandlerID handlerId);

  void UnsubscribeGroup(SubscriberGroup group);

  luabridge::LuaRef NewTable() const;

  void Emmit(defines::Event event, const luabridge::LuaRef &context);

  void SetState(evo::LuaState *l);

  private:
  EventSystem() : nextHandlerId(1) {}

  bool InvokeHandler(const FHandlerSlot &slot, const luabridge::LuaRef &context, defines::Event event);

  evo::LuaState *lua_state = nullptr;

  TArray<FHandlerSlot> handlersByEvent[defines::EventCount];
  TMap<HandlerID, TPair<defines::Event, int32>> handlerLocations;
  HandlerID nextHandlerId;

  public:
  static void lua_reg(lua_State *L) {
    using namespace luabridge;

    // clang-format off
#define register_enum_line(name) .addProperty(#name, []() -> int { return static_cast<int>(defines::Event::name); })
    getGlobalNamespace(L)
      .beginNamespace("defines")
        .beginNamespace("events")
          EVO_EVENT_LIST(register_enum_line)
        .endNamespace()
      .endNamespace()
      .beginClass<EventSystem>("EventSystem") //@class EventSystem
        .addStaticFunction("get", &EventSystem::get)
        .addFunction("sub", [](EventSystem *self, int event, luabridge::LuaRef func) {
          return self->Sub(event, func, NoGroup);
        })
        .addFunction("unsub", &EventSystem::Unsub)
        .addFunction("emmit", &EventSystem::Emmit)
      .endClass();
#undef register_enum_line
    // clang-format on
  }
};
