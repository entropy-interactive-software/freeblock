#pragma once
#include "script_context.hpp"

#include "script_api.hpp"

extern "C" {
#include "lua/lauxlib.h"
#include "lua/lua.h"
};

#include <filesystem.hpp>

#include "console.hpp"
#include "datamodel.hpp"
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

  if (getDM()->isServer()) rdm::Log::printf(rdm::LOG_INFO, "%s", LUA_COPYRIGHT);

  globalState = lua_newstate(l_alloc, this);
}

ScriptThread::~ScriptThread() {
  if (state) lua_close(state);
}

ScriptThread& ScriptContext::newThread() {
  std::string scriptUuid = DataModel::generateUUID();
  ScriptThread th;
  th.state = 0;
  th.instanceUuid = "nil";
  th.source = "";
  th.uuid = scriptUuid;
  th.status = ScriptThread::Stopped;
  threads[scriptUuid] = std::move(th);
  lua_State* L = lua_newthread(globalState);
  threads[scriptUuid].state = L;
  ScriptAPI::add(threads[scriptUuid].state);

  DescribedBridge::pushDescribed(L, getDM()->getRoot());
  lua_setglobal(L, "game");
  DescribedBridge::pushDescribed(
      L, getDM()->getRoot()->getService<WorkspaceInstance>());
  lua_setglobal(L, "workspace");

  return threads[scriptUuid];
}

void ScriptContext::addScript(ScriptInstance* instance) {
  ScriptThread& th = newThread();
  th.instanceUuid = instance->getUUID();
  DescribedBridge::pushDescribed(th.state, instance);
  lua_setglobal(th.state, "script");
}

void ScriptContext::executeScript(std::string source) {
  ScriptThread& th = newThread();
  th.source = source;
  DescribedBridge::pushDescribed(th.state, this);
  lua_setglobal(th.state, "script");
  int error = luaL_loadbuffer(th.state, th.source.c_str(), th.source.size(),
                              source.c_str());
  th.status = ScriptThread::Yielding_PleaseStart;
  if (error) {
    rdm::Log::printf(rdm::LOG_ERROR, "%s", lua_tostring(th.state, -1));
    lua_pop(th.state, 1);
  }
  stepThread(th);
}

void ScriptContext::executeScriptFile(std::string remote) {
  auto d = common::FileSystem::singleton()->readFile(remote.c_str());
  if (!d) throw std::runtime_error("Cant find script");

  ScriptThread& th = newThread();
  th.source = std::string(d.value().begin(), d.value().end());
  DescribedBridge::pushDescribed(th.state, this);
  lua_setglobal(th.state, "script");
  int error =
      luaL_loadbuffer(th.state, th.source.c_str(), th.source.size(), "Exec");
  th.status = ScriptThread::Yielding_PleaseStart;
  if (error) {
    rdm::Log::printf(rdm::LOG_ERROR, "%s", lua_tostring(th.state, -1));
    lua_pop(th.state, 1);
  }
  stepThread(th);
}

void ScriptContext::stepThread(ScriptThread& th) {
  switch (th.status) {
    case ScriptThread::Yielding_PleaseStart:
      try {
        int nr;
        switch (lua_resume(th.state, NULL, 0, &nr)) {
          case LUA_YIELD:
            th.status = ScriptThread::Yielding;
            break;
          case LUA_OK:
            rdm::Log::printf(rdm::LOG_DEBUG, "Script %s stopped",
                             th.uuid.c_str());
            th.status = ScriptThread::Stopped;
            break;
          default:
            rdm::Log::printf(rdm::LOG_ERROR, "%s", lua_tostring(th.state, -1));
            lua_pop(th.state, 1);
            th.status = ScriptThread::Stopped;
            break;
        }
      } catch (std::exception& e) {
        th.status = ScriptThread::Stopped;
      }
      lua_gc(th.state, LUA_GCCOLLECT);
      break;
    case ScriptThread::Yielding:
      th.status = ScriptThread::Yielding_PleaseStart;
      break;
    default:
      break;
  }
}

void ScriptContext::step() {
  for (auto& [uuid, th] : threads) {
    stepThread(th);
  }
}

void ScriptContext::scriptSourceChange(ScriptInstance* instance) {
  auto it = threads.find(instance->getUUID());
  if (it != threads.end()) {
    try {
      ScriptThread& th = it->second;
      int error = luaL_loadbuffer(th.state, instance->getRealSource().c_str(),
                                  instance->getRealSource().size(),
                                  instance->getName().c_str());
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
