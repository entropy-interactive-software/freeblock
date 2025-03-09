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
  static int gc(lua_State* L);

  static int _new(lua_State* L);

 public:
  static void add(lua_State* l);

  static void pushDescribed(lua_State* L, reflection::Described* described);
  static reflection::Described* getDescribed(lua_State* L, unsigned int idx);
};

class Vector3Bridge {
  static int index(lua_State* L);
  static int newindex(lua_State* L);
  static int gc(lua_State* L);

  static int length(lua_State* L);
  static int dot(lua_State* L);

  static int _new(lua_State* L);

  static int _add(lua_State* L);
  static int sub(lua_State* L);
  static int mul(lua_State* L);
  static int div(lua_State* L);
  static int unm(lua_State* L);

  static int eq(lua_State* L);

  static int tostring(lua_State* L);

 public:
  static void add(lua_State* L);

  static void pushVector3(lua_State* L, glm::vec3 v);
  static glm::vec3& getVector3(lua_State* L, unsigned int idx);
};

class ScriptAPI {
  static int print(lua_State* l);

 public:
  static Instance* getScriptObj(lua_State* l);
  static void add(lua_State* l);
};
};  // namespace freeblock
