#pragma once
#include "instance.hpp"
#include "reflection.hpp"
#include "service.hpp"
namespace freeblock {
class DataModelDescribed : public Service {
  DESCRIBED;
  INSTANCE(DataModelDescribed, Service);

  static int luaOpenPlace(lua_State* L);
  static int luaGetCvar(lua_State* L);
  static int luaSetCvar(lua_State* L);
  static int luaGetService(lua_State* L);

 public:
};
};  // namespace freeblock
