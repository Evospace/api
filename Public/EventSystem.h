// Copyright (c) 2017 - 2022, Samsonov Andrey. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ThirdParty/luabridge/LuaBridge.h"
//#include "ThirdParty/lua/lua.h"
#include <string>

namespace evo {
class LuaState;
}

namespace defines {

/**
 * Event types
 */
enum class Event {
  on_player_mined_item,
  on_built_block,
  on_player_spawn,
  on_tick,
  on_player_at_sector
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

  // ...
  default:
    return "unknown_event";
  }
}
} // namespace defines

/**
 * Global event system
 */
class EventSystem {
  public:
  using EventHandler = TFunction<void(luabridge::LuaRef)>;
  using HandlerID = int;

  static EventSystem &GetInstance();
  void LuaCleanup();
  lua_State *L() const;
  evo::LuaState *LuaState() const;

  /**
   * Add callback for event
   * @param event Event type
   * @param func Callback function
   * @return Subscription handle for unsubscription
   */
  HandlerID Sub(int event, luabridge::LuaRef func);

  /**
   * Remove callback from event
   * @param event Event type
   * @param handlerId Subscription handle
   */
  void Unsub(int event, HandlerID handlerId);

  luabridge::LuaRef NewTable() const;

  void Trigger(defines::Event event, const luabridge::LuaRef &context);

  void SetState(evo::LuaState *l);

  private:
  EventSystem()
    : nextHandlerId(1) {}

  evo::LuaState *lua_state = nullptr;

  TMap<defines::Event, TMap<HandlerID, luabridge::LuaRef>> eventHandlers;
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
        .endNamespace()
      .endNamespace()
      .beginClass<EventSystem>("EventSystem") //@class EventSystem
        //direct:
        //--- Get global instance of EventSystem
        //--- @return EventSystem
        //function EventSystem.get_instance() end
        .addStaticFunction("get_instance", &EventSystem::GetInstance)
        //direct:
        //--- Subscribe
        //--- @param event integer Event id
        //--- @param action function Triggering action
        //--- @return integer Subscription id
        //function EventSystem:sub(event, action) end
        .addFunction("sub", &EventSystem::Sub)
        //direct:
        //--- Unsubscribe
        //--- @param event integer Event id
        //--- @param id integer Subscription id
        //function EventSystem:unsub(event, id) end
        .addFunction("unsub", &EventSystem::Unsub)
        .addFunction("trigger", &EventSystem::Trigger)
      .endClass();
#undef register_enum_line
    // clang-format on
  }
};
