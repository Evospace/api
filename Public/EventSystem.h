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

enum class Event {
  on_player_mined_item,
  on_built_block,
  on_player_spawn,
  on_tick,
  on_region_spawn,
  on_player_at_sector,
  on_entity_died,
  on_entity_damaged,
  on_entity_spawn,
  /** Cosmetic day phase boundary (dawn / sunset); context table uses string field `anchor`. */
  on_surface_day_phase,
  Count
};

inline const char *ToString(Event event) {
  switch (event) {
  case Event::on_player_mined_item:
    return "on_player_mined_item";
  case Event::on_built_block:
    return "on_built_block";
  case Event::on_tick:
    return "on_tick";
  case Event::on_player_at_sector:
    return "on_player_at_sector";
  case Event::on_player_spawn:
    return "on_player_spawn";
  case Event::on_region_spawn:
    return "on_region_spawn";
  case Event::on_entity_died:
    return "on_entity_died";
  case Event::on_entity_damaged:
    return "on_entity_damaged";
  case Event::on_entity_spawn:
    return "on_entity_spawn";
  case Event::on_surface_day_phase:
    return "on_surface_day_phase";
  default:
    return "unknown_event";
  }
}

inline bool IsValidEvent(Event event) {
  return event >= Event::on_player_mined_item && event < Event::Count;
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
          register_enum_line(on_player_mined_item)
          register_enum_line(on_built_block)
          register_enum_line(on_player_spawn)
          register_enum_line(on_tick)
          register_enum_line(on_player_at_sector)
          register_enum_line(on_region_spawn)
          register_enum_line(on_surface_day_phase)
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
