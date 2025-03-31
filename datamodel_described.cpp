#include "datamodel_described.hpp"

#include "datamodel.hpp"
#include "instance.hpp"
#include "lua/lauxlib.h"
#include "reflection.hpp"
#include "reflection_props.hpp"
#include "script_api.hpp"
#include "settings.hpp"
namespace freeblock {
INSTANCE_CTOR_REPLICATABLE(DataModelDescribed, Service) {}

int DataModelDescribed::luaOpenPlace(lua_State* L) {
  DataModelDescribed* i =
      dynamic_cast<DataModelDescribed*>(DescribedBridge::getDescribed(L, 1));
  const char* placePath = lua_tostring(L, 2);
  if (lua_gettop(L) == 3) {
    std::string placeType = lua_tostring(L, 3);
    if (placeType == "RBXL") {
      LUA_SAFECALL(L, i->getDM()->loadLegacyMap(placePath));
    } else {
      return luaL_error(L, "Unknown map type", placeType.c_str());
    }
  }
  return 0;
}

int DataModelDescribed::luaGetService(lua_State* L) {
  DataModelDescribed* i =
      dynamic_cast<DataModelDescribed*>(DescribedBridge::getDescribed(L, 1));
  const char* service = lua_tostring(L, 2);

  Instance* s = InstanceFactory::singleton()->getService(service, i->getDM());
  if (!s) return luaL_error(L, "Unknown service %s", service);

  DescribedBridge::pushDescribed(L, s);

  return 1;
}

static rdm::CVar lua_enablesettingcvars("lua_enablesettingcvars", "0");

int DataModelDescribed::luaGetCvar(lua_State* L) {
  const char* cvarName = lua_tostring(L, 1);
  if (rdm::CVar* cvar = rdm::Settings::singleton()->getCvar(cvarName)) {
    lua_pushstring(L, cvar->getValue().c_str());
    return 1;
  } else {
    return luaL_error(L, "Unknown cvar %s", cvarName);
  }
}

int DataModelDescribed::luaSetCvar(lua_State* L) {
  if (!lua_enablesettingcvars.getBool()) {
    return luaL_error(L, "Enable lua_enablesettingcvars to use game:SetCVar");
  }
  const char* cvarName = lua_tostring(L, 1);
  const char* cvarValue = lua_tostring(L, 2);
  if (rdm::CVar* cvar = rdm::Settings::singleton()->getCvar(cvarName)) {
    cvar->setValue(cvarValue);
    return 0;
  } else {
    return luaL_error(L, "Unknown cvar %s", cvarValue);
  }
}

REFLECTION_BEGIN_DESCRIBED(DataModelDescribed);
REFLECTION_FUNCTION(DataModelDescribed, LoadPlace,
                    &DataModelDescribed::luaOpenPlace);
REFLECTION_FUNCTION(DataModelDescribed, GetService,
                    &DataModelDescribed::luaGetService);
REFLECTION_FUNCTION(DataModelDescribed, GetCVar,
                    &DataModelDescribed::luaGetCvar);
REFLECTION_FUNCTION(DataModelDescribed, SetCVar,
                    &DataModelDescribed::luaSetCvar);
REFLECTION_END_DESCRIBED();
}  // namespace freeblock
