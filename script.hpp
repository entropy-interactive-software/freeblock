#pragma once
#include "instance.hpp"
#include "script_context.hpp"
namespace freeblock {
class ScriptInstance : public Instance {
  INSTANCE(ScriptInstance, Instance);
  std::string scriptSource;

 public:
  void setSource(const char* scriptSource);
  const char* getSource() { return scriptSource.c_str(); }
};
}  // namespace freeblock
