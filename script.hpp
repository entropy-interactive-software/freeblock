#pragma once
#include "instance.hpp"
#include "script_context.hpp"
namespace freeblock {
class ScriptInstance : public Instance {
  DESCRIBED;
  INSTANCE(ScriptInstance, Instance);
  std::string scriptSource;
  std::string scriptLinkedSource;

 public:
  void setSource(std::string scriptSource);
  std::string getSource() { return scriptSource; }

  void setLinkedSource(std::string linkedScriptSource);
  std::string getLinkedSource() { return scriptLinkedSource; }

  std::string getRealSource();
};
}  // namespace freeblock
