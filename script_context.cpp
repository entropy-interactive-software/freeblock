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
#include "workspace.hpp"
namespace freeblock {
static void* l_alloc(void* ud, void* ptr, size_t osize, size_t nsize) {
  (void)ud;
  (void)osize; /* not used */
  if (nsize == 0) {
    free(ptr);
    return NULL;
  } else
    return realloc(ptr, nsize);
}

INSTANCE_CTOR_SERVICE(ScriptContext, Service) {
  setName("ScriptContext");
  rdm::Log::printf(rdm::LOG_INFO, "%s", LUA_COPYRIGHT);
  globalState = lua_newstate(l_alloc, this);
}

ScriptThread::~ScriptThread() {
  if (state) lua_close(state);
}

void ScriptContext::addScript(ScriptInstance* instance) {
  ScriptThread th;
  th.state = 0;
  th.uuid = instance->getUUID();
  th.status = ScriptThread::Stopped;
  threads[instance->getUUID()] = std::move(th);
  lua_State* L = lua_newthread(globalState);
  threads[instance->getUUID()].state = L;
  ScriptAPI::add(threads[instance->getUUID()].state);

  DescribedBridge::pushDescribed(L, instance);
  lua_setglobal(L, "script");
  DescribedBridge::pushDescribed(L, getDM()->getRoot());
  lua_setglobal(L, "game");
  DescribedBridge::pushDescribed(
      L, getDM()->getRoot()->getService<WorkspaceInstance>());
  lua_setglobal(L, "workspace");
}

void ScriptContext::step() {
  for (auto& [uuid, th] : threads) {
    switch (th.status) {
      case ScriptThread::Yielding_PleaseStart:
        try {
          int nr;
          switch (lua_resume(th.state, NULL, 0, &nr)) {
            case LUA_YIELD:
              th.status = ScriptThread::Yielding;
              break;
            case LUA_OK:
              th.status = ScriptThread::Stopped;
              break;
            default:
              rdm::Log::printf(rdm::LOG_ERROR, "%s",
                               lua_tostring(th.state, -1));
              lua_pop(th.state, 1);
              th.status = ScriptThread::Stopped;
              break;
          }
        } catch (std::exception& e) {
          th.status = ScriptThread::Stopped;
        }
        break;
      case ScriptThread::Yielding:
        th.status = ScriptThread::Yielding_PleaseStart;
        break;
      default:
        break;
    }
  }
}

void ScriptContext::scriptSourceChange(ScriptInstance* instance) {
  auto it = threads.find(instance->getUUID());
  if (it != threads.end()) {
    try {
      ScriptThread& th = it->second;
      int error = luaL_loadstring(th.state, instance->getRealSource().c_str());
      th.status = ScriptThread::Yielding_PleaseStart;
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
