#include "script.hpp"

#include "instance.hpp"
#include "script_context.hpp"
namespace freeblock {
INSTANCE_CTOR_CREATABLE(ScriptInstance, Instance) {
  ScriptContext* ctxt = getService<ScriptContext>();
  ctxt->addScript(this);
}

void ScriptInstance::setSource(const char* scriptSource) {
  ScriptContext* ctxt = getService<ScriptContext>();
  this->scriptSource = scriptSource;
  ctxt->scriptSourceChange(this);
}
}  // namespace freeblock
