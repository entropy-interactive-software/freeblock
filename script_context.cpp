#pragma once
#include "script_context.hpp"

#include "script_api.hpp"

extern "C" {
#include "lua/lauxlib.h"
#include "lua/lua.h"
};

#include "console.hpp"
#include "logging.hpp"
#include "script.hpp"
namespace freeblock {
INSTANCE_CTOR(ScriptContext, Service) {
  setName("ScriptContext");
  rdm::Log::printf(rdm::LOG_INFO, "%s", LUA_COPYRIGHT);
}

ScriptThread::~ScriptThread() {
  if (state) lua_close(state);
}

static void* l_alloc(void* ud, void* ptr, size_t osize, size_t nsize) {
  (void)ud;
  (void)osize; /* not used */
  if (nsize == 0) {
    free(ptr);
    return NULL;
  } else
    return realloc(ptr, nsize);
}

void ScriptContext::addScript(ScriptInstance* instance) {
  ScriptThread th;
  th.state = 0;
  th.uuid = instance->getUUID();
  threads[instance->getUUID()] = std::move(th);
  lua_State* L = lua_newstate(l_alloc, &threads[instance->getUUID()]);
  threads[instance->getUUID()].state = L;
  ScriptAPI::add(threads[instance->getUUID()].state);

  DescribedBridge::pushDescribed(L, instance);
  lua_setglobal(L, "script");
  DescribedBridge::pushDescribed(L, getDM()->getRoot());
  lua_setglobal(L, "game");
}

void ScriptContext::step() {}

void ScriptContext::scriptSourceChange(ScriptInstance* instance) {
  auto it = threads.find(instance->getUUID());
  if (it != threads.end()) {
    try {
      ScriptThread& th = it->second;
      int error = luaL_loadstring(th.state, instance->getSource());
      if (error) {
        rdm::Log::printf(rdm::LOG_ERROR, "%s", lua_tostring(th.state, -1));
        lua_pop(th.state, 1);
      }
      error = lua_pcall(th.state, 0, 0, 0);
      if (error) {
        rdm::Log::printf(rdm::LOG_ERROR, "%s", lua_tostring(th.state, -1));
        lua_pop(th.state, 1);
      }
    } catch (std::exception& e) {
      rdm::Log::printf(rdm::LOG_ERROR, "%s", e.what());
    }
  }
}
};  // namespace freeblock
