#pragma once
#include "datamodel.hpp"

extern "C" {
#include "lua/lua.h"
};

#include "script.hpp"
#include "service.hpp"
namespace freeblock {
class DataModel;
class ScriptInstance;

struct ScriptThread {
  enum Status {
    Yielding,
    Yielding_PleaseStart,
    Stopped,
  } status;

  lua_State* state;
  std::string uuid;
  InstanceUUID instanceUuid;
  std::string source;

  ~ScriptThread();
};

class ScriptContext : public Service {
  INSTANCE(ScriptContext, Service);

  std::map<InstanceUUID, ScriptThread> threads;
  lua_State* globalState;

  ScriptThread& newThread();
  void stepThread(ScriptThread& thr);

 public:
  void addScript(ScriptInstance* instance);

  void executeScriptFile(std::string remote);
  void executeScript(std::string source);
  void scriptSourceChange(ScriptInstance* instance);

  ScriptThread& getThread(InstanceUUID uuid) {
    auto it = threads.find(uuid);
    if (it != threads.end())
      return it->second;
    else
      throw std::runtime_error("Invalid thread");
  }

  virtual void step();
};
};  // namespace freeblock
