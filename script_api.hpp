#pragma once
#include "instance.hpp"
#include "reflection.hpp"
extern "C" {
#include "lua/lauxlib.h"
#include "lua/lua.h"
}
namespace freeblock {
class DescribedBridge {
  static int index(lua_State* L);
  static int newindex(lua_State* L);

 public:
  static void add(lua_State* l);

  static void pushDescribed(lua_State* L, reflection::Described* described);
  static reflection::Described* getDescribed(lua_State* L, unsigned int idx);
};

class ScriptAPI {
  static int print(lua_State* l);

 public:
  Instance* getScriptObj(lua_State* l);
  static void add(lua_State* l);
};
};  // namespace freeblock
