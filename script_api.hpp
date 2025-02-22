#pragma once
extern "C" {
#include "lua/lauxlib.h"
#include "lua/lua.h"
}
namespace freeblock {
class ScriptAPI {
 public:
  static void add(lua_State* l);
};
};  // namespace freeblock
