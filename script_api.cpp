#include "script_api.hpp"

#include "instance.hpp"
#include "logging.hpp"
#include "reflection.hpp"
#include "reflection_props.hpp"
extern "C" {
#include "lua/lauxlib.h"
#include "lua/lua.h"
}

namespace freeblock {

int DescribedBridge::index(lua_State* L) {
  reflection::Described* object = getDescribed(L, 1);
  const char* name = lua_tostring(L, 2);

  reflection::PropertyList plist = object->getProperties();
  auto it = plist.find(name);
  if (it != plist.end()) {
    reflection::Property* p = it->second;
    switch (p->getType()) {
      case reflection::Property::String:
        lua_pushstring(L, p->getString(object).c_str());
        break;
      case reflection::Property::InstanceRef:
        if (Instance* v = p->getInstance(object)) {
          DescribedBridge::pushDescribed(L, v);
        } else {
          lua_pushnil(L);
        }
        break;
      default:
        rdm::Log::printf(rdm::LOG_ERROR, "Attempted access on property %s",
                         name);
        throw std::runtime_error(
            "Invalid access on property (DEVELOPER FIXME)");
        break;
    }
    return 1;
  }

  if (Instance* instance = dynamic_cast<Instance*>(object)) {
    if (Instance* find = instance->findFirstChildOfName(name)) {
      DescribedBridge::pushDescribed(L, find);
      return 1;
    }
  }

  rdm::Log::printf(rdm::LOG_ERROR, "Attempted access on property %s", name);
  throw std::runtime_error("Invalid access on property");
}

int DescribedBridge::newindex(lua_State* L) {
  reflection::Described* object = getDescribed(L, 1);
  const char* name = lua_tostring(L, 2);

  reflection::PropertyList plist = object->getProperties();
  auto it = plist.find(name);
  if (it != plist.end()) {
    reflection::Property* p = it->second;
    switch (p->getType()) {
      case reflection::Property::String:
        p->setString(object, lua_tostring(L, 3));
        break;
      case reflection::Property::InstanceRef:
        if (lua_isnil(L, 3)) {
          p->setInstance(object, NULL);
          break;
        }
        if (Instance* i =
                dynamic_cast<Instance*>(DescribedBridge::getDescribed(L, 3))) {
          p->setInstance(object, i);
        } else {
          throw std::runtime_error("Attempt to set to a nil value");
        }
        break;
      default:
        rdm::Log::printf(rdm::LOG_ERROR, "Attempted access on property %s",
                         name);
        throw std::runtime_error(
            "Invalid access on property (DEVELOPER FIXME)");
        break;
    }
    return 0;
  }

  rdm::Log::printf(rdm::LOG_ERROR, "Attempted access on property %s", name);
  throw std::runtime_error("Invalid access on property");
}

void DescribedBridge::add(lua_State* L) {
  // luaL_Reg reg[] = {{"__eq", __eq}, {NULL, NULL}};

  luaL_newmetatable(L, "Described");

  lua_pushstring(L, "type");
  lua_pushstring(L, "Described");
  lua_settable(L, -3);

  lua_pushstring(L, "__index");
  lua_pushcfunction(L, index);
  lua_settable(L, -3);

  lua_pushstring(L, "__newindex");
  lua_pushcfunction(L, newindex);
  lua_settable(L, -3);

  lua_pop(L, 1);
}

reflection::Described* DescribedBridge::getDescribed(lua_State* L,
                                                     unsigned int idx) {
  void** ud = (void**)luaL_checkudata(L, idx, "Described");
  return (reflection::Described*)(*ud);
}

void DescribedBridge::pushDescribed(lua_State* L,
                                    reflection::Described* described) {
  reflection::Described** value = (reflection::Described**)lua_newuserdata(
      L, sizeof(reflection::Described*));
  *value = described;
  luaL_getmetatable(L, "Described");
  lua_setmetatable(L, -2);
}

int ScriptAPI::print(lua_State* L) {
  const char* str = lua_tostring(L, -1);
  rdm::Log::printf(rdm::LOG_INFO, "%s", str);
  return 0;
}

Instance* ScriptAPI::getScriptObj(lua_State* L) {
  lua_getglobal(L, "script");
  return dynamic_cast<Instance*>(DescribedBridge::getDescribed(L, 1));
}

void ScriptAPI::add(lua_State* L) {
  DescribedBridge::add(L);

  lua_pushcfunction(L, &ScriptAPI::print);
  lua_setglobal(L, "print");
}
};  // namespace freeblock
