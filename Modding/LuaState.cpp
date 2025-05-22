// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.
#include "LuaState.h"

#include "StaticLogger.h"
#include "LuaRegistration/ActorComponents.h"
#include "LuaRegistration/Math.h"
#include "LuaRegistration/MiscEvospace.h"
#include "LuaRegistration/ModApi.h"

namespace evo {

void LuaState::AddLuaPath(const FString &path) {
  lua_getglobal(L, "package");
  lua_getfield(L, -1, "path");
  FString cur_path = UTF8_TO_TCHAR(lua_tostring(L, -1));
  cur_path = cur_path + ";" + path + "/?.lua";
  lua_pop(L, 1);
  lua_pushstring(L, TCHAR_TO_UTF8(*cur_path));
  lua_setfield(L, -2, "path");
  lua_pop(L, 1);
}

bool LuaState::RunCode(lua_State *L, std::string_view code, std::string_view path) {
  std::string path_decorated = std::string("@") + path.data();
  if (luaL_loadbuffer(L, code.data(), code.size(), path_decorated.data())) {
    LOG(ERROR_LL) << "Lua loading error: " << UTF8_TO_TCHAR(lua_tostring(L, -1));
    return false;
  }

  if (lua_pcall(L, 0, LUA_MULTRET, 0)) {
    HandleLuaErrorOnStack(L);
    return false;
  }

  return true;
}

bool LuaState::RunCode(std::string_view code, std::string_view path) const {
  return RunCode(L, code, path);
}

std::vector<std::string> split(std::string_view s, char delim) {
  std::vector<std::string> result;
  std::stringstream ss(s.data());
  std::string item;

  while (getline(ss, item, delim)) {
    result.push_back(item);
  }

  return result;
}

int LuaState::ToByteCode_Writer(
  lua_State *L, const void *ptr, size_t size, void *user_data) {
  const auto output = static_cast<std::string *>(user_data);
  const auto ptr_u8 = static_cast<const uint8_t *>(ptr);

  output->insert(output->end(), ptr_u8, ptr_u8 + size);
  return 0;
}

std::string LuaState::ToByteCode(std::string_view code, std::string_view path) {
  std::string output;
  std::string path_decorated = std::string("@") + path.data();
  lua_State *L = luaL_newstate();
  if (auto err = luaL_loadbuffer(
        L, code.data(), code.length(), path_decorated.data())) {
    std::string error = lua_tostring(L, -1);
    lua_close(L);
    auto split_error = split(error, ':');
    auto index = std::stoi(split_error[1]);
    auto split_source = split(code, '\n');
    auto in_error = split_source[index - 1];
    LOG(ERROR_LL) << "Load buffer error: " << UTF8_TO_TCHAR(error.data()) << "; line " << index
                  << ": " << UTF8_TO_TCHAR(in_error.data());
    return "";
  }

  if (lua_dump(L, ToByteCode_Writer, &output)) {
    auto error = lua_tostring(L, -1);
    lua_close(L);
    LOG(ERROR_LL) << "Dump error: " << UTF8_TO_TCHAR(error);
    return "";
  }

  lua_close(L);
  return output;
}

void LuaState::HandleLuaErrorOnStackWithSource(std::string_view code) const {
  std::string error = lua_tostring(L, -1);
  auto split_error = split(error, ':');
  auto index = std::stoi(split_error[1]);
  auto split_source = split(code, '\n');
  auto in_error = split_source[index - 1];
  LOG(ERROR_LL) << "Load buffer error: " << UTF8_TO_TCHAR(error.data()) << "; line " << index
                << ": " << UTF8_TO_TCHAR(in_error.data());
  lua_pop(L, 1);
}

std::string LuaState::DumpLuaFunction(const luabridge::LuaRef &lua_function) {
  if (lua_function.isFunction()) {
    lua_State *L = lua_function.state();

    auto string_module = luabridge::getGlobal(L, "string");
    auto string_dump = string_module["dump"];
    auto result = string_dump(lua_function);
    if (HandleLuaResult(L, result)) {
      return result[0].tostring();
    }
  }

  return "<lua dump error>";
}

luabridge::LuaRef LuaState::ToLuaRefFunction(std::string_view code, std::string_view path) const {
  std::string output;
  std::string path_decorated = std::string("@") + path.data();
  if (auto err = luaL_loadbuffer(L, code.data(), code.length(), path_decorated.data())) {
    HandleLuaErrorOnStackWithSource(code);
    return luabridge::LuaRef(L, nullptr);
  }

  luabridge::LuaRef funcRef = luabridge::LuaRef::fromStack(L, -1);
  lua_pop(L, 1);

  return funcRef;
}

bool LuaState::HandleLuaResult(const luabridge::LuaResult &res) const {
  return HandleLuaResult(L, res);
}

bool LuaState::HandleLuaResult(lua_State *L, const luabridge::LuaResult &res) {
  if (res.wasOk())
    return true;

  LOG(ERROR_LL) << "Lua result error: " << UTF8_TO_TCHAR(res.errorMessage().data());

  LOG(ERROR_LL) << "Call stack:";
  int level = 0;
  lua_Debug debug_info;
  while (lua_getstack(L, level, &debug_info)) {
    lua_getinfo(L, "nSlf", &debug_info);
    LOG(ERROR_LL) << "    " << UTF8_TO_TCHAR(debug_info.short_src) << ":"
                  << debug_info.currentline;
    if (debug_info.name != nullptr)
      LOG(ERROR_LL) << " in function " << UTF8_TO_TCHAR(debug_info.name);
    ++level;
  }

  return false;
}

void LuaState::HandleLuaErrorOnStack(lua_State *L) {
  LOG(ERROR_LL) << "Lua execution error: " << UTF8_TO_TCHAR(lua_tostring(L, -1));

  LOG(ERROR_LL) << "Call stack:";
  int level = 0;
  lua_Debug debug_info;
  while (lua_getstack(L, level, &debug_info)) {
    lua_getinfo(L, "nSlf", &debug_info);
    LOG(ERROR_LL) << "    " << UTF8_TO_TCHAR(debug_info.short_src) << ":"
                  << debug_info.currentline;
    if (debug_info.name != nullptr)
      LOG(ERROR_LL) << " in function " << UTF8_TO_TCHAR(debug_info.name);
    ++level;
  }
}

void LuaState::HandleLuaErrorOnStack() const {
  HandleLuaErrorOnStack(L);
}

template <ELogLevel Severity>
int LuaState::l_my_print(lua_State *L) {
  int nargs = lua_gettop(L);

  for (int i = 1; i <= nargs; i++) {
    if (lua_isstring(L, i)) {
      LOG(Severity) << "Lua: " << UTF8_TO_TCHAR(lua_tostring(L, i));
    } else if (lua_isnumber(L, i)) {
      LOG(Severity) << "Lua: " << lua_tonumber(L, i);
    } else if (lua_isboolean(L, i)) {
      LOG(Severity) << "Lua: " << lua_toboolean(L, i);
    } else if (lua_isnil(L, i)) {
      LOG(Severity) << "Lua: nil";
    } else {
      LOG(Severity) << "Lua: print not implemented type";
    }
  }

  return 0;
}

LuaState::LuaState() {
  L = luaL_newstate();

  luaL_openlibs(L);

  luabridge::setGlobal(L, nullptr, "os");
  luabridge::setGlobal(L, nullptr, "coroutine");
  luabridge::setGlobal(L, nullptr, "io");
  luabridge::setGlobal(L, nullptr, "utf8");

  luabridge::setGlobal(L, &LuaState::l_my_print<INFO_LL>, "print");
  luabridge::setGlobal(L, &LuaState::l_my_print<ERROR_LL>, "print_err");
  luabridge::setGlobal(L, &LuaState::l_my_print<WARN_LL>, "print_warn");

  auto col = luabridge::getGlobal(L, "collectgarbage");
  col("setpause", 100);

  LOG(INFO_LL) << "Lua state is constructed";
}

void LuaState::Init(IRegistrar *registrar) {
  LOG(INFO_LL) << "Lua actor component registering";

  registerComponentClasses(L);

  LOG(INFO_LL) << "Lua extra math registering";

  registerMathClasses(L);

  LOG(INFO_LL) << "Lua state initialized";

  auto ver = luabridge::getGlobal(L, "_VERSION");
  LOG(INFO_LL) << UTF8_TO_TCHAR(ver.tostring().data());

  std::ignore = RunCode(
    "require('jit') if type(jit) == 'table' then print(jit.version) else "
    "print('jit fatal error') end",
    "@jit_test");
}

void LuaState::Release() {
}

void LuaState::LuaClose() {
  lua_close(L);
  L = nullptr;
}

UClass *LuaState::FindClass(const std::string &name) {
  auto type = FindObject<UClass>(ANY_PACKAGE, UTF8_TO_TCHAR(name.data()));

  if (type == nullptr) {
    LOG(ERROR_LL) << "Class not found " << UTF8_TO_TCHAR(name.data());
  } else {
    //LOG(TRACE_LL) << TCHAR_TO_UTF8(*type->GetName()) << " is loaded";
  }

  return type;
}

UClass *LuaState::LoadClass(const std::string &name) {
  auto type = LoadObject<UClass>(nullptr, UTF8_TO_TCHAR(name.data()));

  if (type == nullptr) {
    LOG(ERROR_LL) << "Class not loaded " << UTF8_TO_TCHAR(name.data());
  } else {
    //LOG(TRACE_LL) << TCHAR_TO_UTF8(*type->GetName()) << " is loaded";
  }

  return type;
}

UTexture2D *LuaState::FindTexture(const std::string &name) {
  auto type = FindObject<UTexture2D>(ANY_PACKAGE, UTF8_TO_TCHAR(name.data()));

  if (type == nullptr) {
    LOG(WARN_LL) << "Texture not found " << UTF8_TO_TCHAR(name.data());
  } else {
    //LOG(TRACE_LL) << TCHAR_TO_UTF8(*type->GetName()) << " is loaded";
  }

  return type;
}

int LuaState::errorHandler(lua_State *L) {
  // Get the error message from the stack
  const char *errorMessage = lua_tostring(L, -1);

  // Create a traceback
  lua_getglobal(L, "debug");
  lua_getfield(L, -1, "traceback");
  lua_pushstring(L, errorMessage);
  lua_pushinteger(L, 1); // Skip this function in the traceback

  // Call debug.traceback
  lua_call(L, 2, 1);

  LOG(ERROR_LL) << lua_tostring(L, -1);

  // Return the traceback message
  return 1;
}

LuaState::CallResult LuaState::DebugCall(const luabridge::LuaRef &function) const {
  lua_pushcfunction(L, errorHandler);
  function.push(L);
  int result = luabridge::pcall(L, 0, LUA_MULTRET, -2);
  return result == 0 ? CallResult::Success : CallResult::Error;
}

LuaState::~LuaState() {
  if (L != nullptr) {

    lua_close(L);
    L = nullptr;
  }
}
} // namespace evo