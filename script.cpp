#include "script.hpp"

#include "filesystem.hpp"
#include "instance.hpp"
#include "reflection.hpp"
#include "reflection_props.hpp"
#include "script_context.hpp"
namespace freeblock {
INSTANCE_CTOR_CREATABLE(ScriptInstance, Instance) {
  setName("Script");
  ScriptContext* ctxt = getService<ScriptContext>();
  ctxt->addScript(this);
  scriptLinkedSource = "";
}

REFLECTION_BEGIN_DESCRIBED(ScriptInstance)
REFLECTION_PROPERTY_STRING(ScriptInstance, Source, &ScriptInstance::getSource,
                           &ScriptInstance::setSource);
REFLECTION_PROPERTY_STRING(ScriptInstance, LinkedSource,
                           &ScriptInstance::getLinkedSource,
                           &ScriptInstance::setLinkedSource);
REFLECTION_END_DESCRIBED();

void ScriptInstance::setSource(std::string scriptSource) {
  ScriptContext* ctxt = getService<ScriptContext>();
  this->scriptSource = scriptSource;
  ctxt->scriptSourceChange(this);
}

void ScriptInstance::setLinkedSource(std::string linkedScriptSource) {
  ScriptContext* ctxt = getService<ScriptContext>();
  this->scriptLinkedSource = linkedScriptSource;
  ctxt->scriptSourceChange(this);
}

std::string ScriptInstance::getRealSource() {
  if (scriptLinkedSource.empty())
    return scriptSource;
  else {
    auto f =
        common::FileSystem::singleton()->readFile(scriptLinkedSource.c_str());
    if (f.has_value()) {
      auto d = f.value();
      return std::string(d.begin(), d.end());
    } else {
      rdm::Log::printf(rdm::LOG_WARN,
                       "Could not retrieve linked source %s for script %s (%s)",
                       scriptLinkedSource.c_str(), getName().c_str(),
                       getUUID().c_str());
      return "";
    }
  }
}
}  // namespace freeblock
