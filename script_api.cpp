#include "script_api.hpp"

#include <format>

#include "instance.hpp"
#include "logging.hpp"
#include "reflection.hpp"
#include "reflection_props.hpp"
extern "C" {
#include "lua/lauxlib.h"
#include "lua/lua.h"
#include "lua/lualib.h"
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
      case reflection::Property::Function: {
        lua_CFunction m =
            *(p->getFunction().target<reflection::LuaFunctionT*>());
        if (!m)
          throw std::runtime_error(
              "p->getFunction().target<reflection::LuaFunctionT*> returned "
              "null");
        lua_pushcfunction(L, m);
      } break;
      case reflection::Property::Bool: {
        lua_pushboolean(L, p->getBool(object));
      } break;
      case reflection::Property::Vec3: {
        Vector3Bridge::pushVector3(L, p->getVec3(object));
      } break;
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
      case reflection::Property::Vec3:
        p->setVec3(object, Vector3Bridge::getVector3(L, 3));
        break;
      case reflection::Property::Bool:
        p->setBool(object, lua_toboolean(L, 3));
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

int DescribedBridge::gc(lua_State* L) {
  // void** ud = (void**)luaL_checkudata(L, 1, "Described");
  // rdm::Log::printf(rdm::LOG_ERROR, "gc");
  return 0;
}

void DescribedBridge::add(lua_State* L) {
  static const struct luaL_Reg lib[] = {{"new", _new}, {NULL, NULL}};

  luaL_newmetatable(L, "Described");

  lua_pushstring(L, "__metatable");
  lua_pushstring(L, "NUNYA");
  lua_settable(L, -3);

  lua_pushstring(L, "type");
  lua_pushstring(L, "Described");
  lua_settable(L, -3);

  lua_pushstring(L, "__index");
  lua_pushcfunction(L, index);
  lua_settable(L, -3);

  lua_pushstring(L, "__newindex");
  lua_pushcfunction(L, newindex);
  lua_settable(L, -3);

  lua_pushstring(L, "__gc");
  lua_pushcfunction(L, gc);
  lua_settable(L, -3);

  lua_pop(L, 1);

  lua_newtable(L);
  luaL_setfuncs(L, lib, 0);
  lua_setglobal(L, "Instance");
}

int DescribedBridge::_new(lua_State* L) {
  Instance* script = ScriptAPI::getScriptObj(L);
  const char* type = lua_tostring(L, 1);

  if (lua_gettop(L) == 1) {
    Instance* i = InstanceFactory::singleton()->create(type, script->getDM());
    if (!i) throw std::runtime_error("Invalid instance");
    pushDescribed(L, i);
  } else if (lua_gettop(L) == 2) {
    Instance* p = dynamic_cast<Instance*>(DescribedBridge::getDescribed(L, 2));
    Instance* i = InstanceFactory::singleton()->create(type, script->getDM());
    if (!i) throw std::runtime_error("Invalid instance");
    i->setParent(p);
    pushDescribed(L, i);
  } else {
    throw std::runtime_error("");
  }

  return 1;
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

int Vector3Bridge::index(lua_State* L) {
  glm::vec3 v = getVector3(L, 1);
  std::string name = lua_tostring(L, 2);

  if (name == "x") {
    lua_pushnumber(L, v.x);
  } else if (name == "y") {
    lua_pushnumber(L, v.y);
  } else if (name == "z") {
    lua_pushnumber(L, v.z);
  } else if (name == "length") {
    lua_pushcfunction(L, length);
  } else if (name == "dot") {
    lua_pushcfunction(L, dot);
  } else {
    throw std::runtime_error("Invalid access on Vector3");
  }

  return 1;
}

int Vector3Bridge::newindex(lua_State* L) {
  glm::vec3& v = getVector3(L, 1);
  std::string name = lua_tostring(L, 2);

  if (name == "x") {
    v.x = lua_tonumber(L, 3);
  } else if (name == "y") {
    v.y = lua_tonumber(L, 3);
  } else if (name == "z") {
    v.z = lua_tonumber(L, 3);
  }

  return 0;
}

int Vector3Bridge::gc(lua_State* L) { return 0; }

int Vector3Bridge::length(lua_State* L) {
  glm::vec3 v = getVector3(L, 1);
  lua_pushnumber(L, glm::length(v));
  return 1;
}

int Vector3Bridge::dot(lua_State* L) {
  lua_pushnumber(L, glm::dot(getVector3(L, 1), getVector3(L, 2)));
  return 1;
}

int Vector3Bridge::_new(lua_State* L) {
  glm::vec3 v;
  if (lua_gettop(L) == 0) {
    v = glm::vec3(0.0);
  } else if (lua_gettop(L) == 1) {
    v = glm::vec3(1.0) * (float)lua_tonumber(L, 1);
  } else if (lua_gettop(L) == 3) {
    v = glm::vec3(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3));
  }
  pushVector3(L, v);
  return 1;
}

int Vector3Bridge::_add(lua_State* L) {
  pushVector3(L, getVector3(L, 1) + getVector3(L, 2));
  return 1;
}

int Vector3Bridge::sub(lua_State* L) {
  pushVector3(L, getVector3(L, 1) - getVector3(L, 2));
  return 1;
}

int Vector3Bridge::mul(lua_State* L) {
  if (lua_isnumber(L, 2))
    pushVector3(L, getVector3(L, 1) * (float)lua_tonumber(L, 2));
  else
    pushVector3(L, getVector3(L, 1) * getVector3(L, 2));
  return 1;
}

int Vector3Bridge::div(lua_State* L) {
  if (lua_isnumber(L, 2))
    pushVector3(L, getVector3(L, 1) / (float)lua_tonumber(L, 2));
  else
    pushVector3(L, getVector3(L, 1) / getVector3(L, 2));
  return 1;
}

int Vector3Bridge::unm(lua_State* L) {
  pushVector3(L, -getVector3(L, 1));
  return 1;
}

int Vector3Bridge::eq(lua_State* L) {
  lua_pushboolean(L, getVector3(L, 1) == getVector3(L, 2));
  return 1;
}

int Vector3Bridge::tostring(lua_State* L) {
  glm::vec3 v = getVector3(L, 1);
  std::string s = std::format("{}, {}, {}", v.x, v.y, v.z);
  lua_pushstring(L, s.c_str());
  return 1;
}

void Vector3Bridge::add(lua_State* L) {
  static const struct luaL_Reg vec3lib[] = {
      {"new", _new}, {"dot", dot}, {"length", length}, {NULL, NULL}};

  luaL_newmetatable(L, "Vector3");

  lua_pushstring(L, "__metatable");
  lua_pushstring(L, "NUNYA");
  lua_settable(L, -3);

  lua_pushstring(L, "type");
  lua_pushstring(L, "Vector3");
  lua_settable(L, -3);

  lua_pushstring(L, "__index");
  lua_pushcfunction(L, index);
  lua_settable(L, -3);

  lua_pushstring(L, "__newindex");
  lua_pushcfunction(L, newindex);
  lua_settable(L, -3);

  lua_pushstring(L, "__gc");
  lua_pushcfunction(L, gc);
  lua_settable(L, -3);

  lua_pushstring(L, "__add");
  lua_pushcfunction(L, _add);
  lua_settable(L, -3);

  lua_pushstring(L, "__sub");
  lua_pushcfunction(L, sub);
  lua_settable(L, -3);

  lua_pushstring(L, "__mul");
  lua_pushcfunction(L, mul);
  lua_settable(L, -3);

  lua_pushstring(L, "__div");
  lua_pushcfunction(L, div);
  lua_settable(L, -3);

  lua_pushstring(L, "__unm");
  lua_pushcfunction(L, unm);
  lua_settable(L, -3);

  lua_pushstring(L, "__eq");
  lua_pushcfunction(L, eq);
  lua_settable(L, -3);

  lua_pushstring(L, "__tostring");
  lua_pushcfunction(L, tostring);
  lua_settable(L, -3);

  lua_pop(L, 1);

  lua_newtable(L);
  luaL_setfuncs(L, vec3lib, 0);
  lua_setglobal(L, "Vector3");
}

void Vector3Bridge::pushVector3(lua_State* L, glm::vec3 v) {
  glm::vec3* value = (glm::vec3*)lua_newuserdata(L, sizeof(glm::vec3));
  *value = v;
  luaL_getmetatable(L, "Vector3");
  lua_setmetatable(L, -2);
}

glm::vec3& Vector3Bridge::getVector3(lua_State* L, unsigned int idx) {
  void* ud = (void*)luaL_checkudata(L, idx, "Vector3");
  return *(glm::vec3*)(ud);
}

int ScriptAPI::print(lua_State* L) {
  for (int i = 0; i < lua_gettop(L); i++) {
    const char* str = luaL_tolstring(L, i + 1, NULL);
    rdm::Log::printf(rdm::LOG_INFO, "%s", str);
    lua_pop(L, 1);
  }
  return 0;
}

int ScriptAPI::wait(lua_State* L) {
  getScriptThread(L).status = ScriptThread::Yielding;
  return lua_yield(L, 0);
}

ScriptThread& ScriptAPI::getScriptThread(lua_State* L) {
  Instance* i = getScriptObj(L);
  ScriptContext* context = i->getDM()->getRoot()->getService<ScriptContext>();
  return context->getThread(i->getUUID());
}

Instance* ScriptAPI::getScriptObj(lua_State* L) {
  lua_getglobal(L, "script");
  Instance* i = dynamic_cast<Instance*>(DescribedBridge::getDescribed(L, -1));
  lua_pop(L, 1);
  return i;
}

void ScriptAPI::add(lua_State* L) {
  DescribedBridge::add(L);
  Vector3Bridge::add(L);

  luaL_requiref(L, LUA_MATHLIBNAME, luaopen_math, 1);
  lua_pop(L, 1);

  lua_pushcfunction(L, &ScriptAPI::print);
  lua_setglobal(L, "print");

  lua_pushcfunction(L, &ScriptAPI::wait);
  lua_setglobal(L, "wait");
}
};  // namespace freeblock
