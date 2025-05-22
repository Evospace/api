// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#pragma once
#include "Public/DB.h"
#include "ThirdParty/luabridge/luabridge.h"

#include <string>

struct lua_State;

namespace evo {

/**
 * @brief Most basic lua state class
 */
class LuaState {
  public:
  template <ELogLevel Severity>
  static int l_my_print(lua_State *L);

  LuaState();

  virtual void Init(IRegistrar *registrar);
  virtual void Release();
  void LuaClose();

  LuaState(const LuaState &v) = delete;
  LuaState &operator=(const LuaState &v) = delete;

  static UClass *FindClass(const std::string &name);
  static UClass *LoadClass(const std::string &name);
  static UTexture2D *FindTexture(const std::string &name);
  template <typename _Ty>
  static _Ty *LuaLoadObject(const std::string &name) {
    auto type = LoadObject<_Ty>(nullptr, UTF8_TO_TCHAR(name.data()));

    if (type == nullptr) {
      LOG(ERROR_LL) << _Ty::StaticClass()->GetName() << " not found " << UTF8_TO_TCHAR(name.data());
    } else {
      //LOG(TRACE_LL) << TCHAR_TO_UTF8(*type->GetName()) << " is loaded";
    }

    return type;
  }
  template <typename _Ty>
  static _Ty *LuaFindObject(const std::string &name) {
    auto type = FindObject<_Ty>(ANY_PACKAGE, UTF8_TO_TCHAR(name.data()));

    if (type == nullptr) {
      LOG(ERROR_LL) << _Ty::StaticClass()->GetName() << " not found " << UTF8_TO_TCHAR(name.data());
    } else {
      //LOG(TRACE_LL) << TCHAR_TO_UTF8(*type->GetName()) << " is loaded";
    }

    return type;
  }

  static int errorHandler(lua_State *L);

  enum class CallResult : uint8 {
    Success,
    Error
  };
  CallResult DebugCall(const luabridge::LuaRef &function) const;

  template <typename _Ty>
  CallResult DebugCall(const luabridge::LuaRef &function, const _Ty &arg) const {
    lua_pushcfunction(L, errorHandler);
    function.push(L);
    luabridge::Stack<_Ty>::push(L, arg);
    int result = luabridge::pcall(L, 1, LUA_MULTRET, -2);
    return result == 0 ? CallResult::Success : CallResult::Error;
  }

  template <typename _Ty, typename _Ty2>
  CallResult DebugCall(const luabridge::LuaRef &function, const _Ty &arg, const _Ty2 &arg2) const {
    lua_pushcfunction(L, errorHandler);
    function.push(L);
    luabridge::Stack<_Ty>::push(L, arg);
    luabridge::Stack<_Ty2>::push(L, arg2);
    int result = luabridge::pcall(L, 2, LUA_MULTRET, -2);
    return result == 0 ? CallResult::Success : CallResult::Error;
  }

  template <typename... Args>
  static void CallAndHandleError(const luabridge::LuaRef &code, Args &&...args) {
    if (code.isFunction()) {
      std::ignore = HandleLuaResult(code.state(), (code)(std::forward<Args>(args)...));
    }
  }

  template <typename... Args>
  static void CallAndHandleError(const std::optional<luabridge::LuaRef> &code, Args &&...args) {
    if (code.has_value() && code->isFunction()) {
      std::ignore = HandleLuaResult(code->state(), (*code)(std::forward<Args>(args)...));
    }
  }

  void AddLuaPath(const FString &path);

  void doFile(std::string_view s) { luaL_dofile(L, s.data()); }

  virtual ~LuaState();

  lua_State *L = nullptr;

  static int ToByteCode_Writer(lua_State *L, const void *ptr, size_t size, void *user_data);
  static std::string ToByteCode(std::string_view code, std::string_view path);
  void HandleLuaErrorOnStackWithSource(std::string_view code) const;

  static std::string DumpLuaFunction(const luabridge::LuaRef &lua_function);

  luabridge::LuaRef ToLuaRefFunction(std::string_view code, std::string_view path) const;

  bool HandleLuaResult(const luabridge::LuaResult &res) const;
  static bool HandleLuaResult(lua_State *L, const luabridge::LuaResult &res);

  void HandleLuaErrorOnStack() const;
  static void HandleLuaErrorOnStack(lua_State *L);

  /**
     * @brief
     * @param code string with lua code; it will be running in all loaded
     * context in this lua state
     * @param CodePath label in error log for this code fragment execution
     * @return true if there is no errors
     */
  bool RunCode(std::string_view code, std::string_view CodePath = "") const;
  static bool RunCode(lua_State *L, std::string_view code, std::string_view path);
};
} // namespace evo