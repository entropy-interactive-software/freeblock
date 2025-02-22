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
  lua_State* state;
  InstanceUUID uuid;

  ~ScriptThread();
};

class ScriptContext : public Service {
  INSTANCE(ScriptContext, Service);

  std::map<InstanceUUID, ScriptThread> threads;

 public:
  void addScript(ScriptInstance* instance);
  void scriptSourceChange(ScriptInstance* instance);
};
};  // namespace freeblock
