// lua.hpp
// Lua header files for C++
// <<extern "C">> not supplied automatically because Lua also compiles as C++

extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "luacompat.h"
#include "lualib.h"
}
