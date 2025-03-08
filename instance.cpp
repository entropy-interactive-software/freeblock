#include "instance.hpp"

#include "datamodel.hpp"
#include "logging.hpp"
#include "reflection.hpp"
#include "reflection_props.hpp"
#include "script_api.hpp"
namespace freeblock {
static InstanceFactory* _singleton = 0;
InstanceFactory* InstanceFactory::singleton() {
  if (!_singleton) _singleton = new InstanceFactory;
  return _singleton;
}

REFLECTION_BEGIN_DESCRIBED(Instance);
REFLECTION_PROPERTY_STRING(Instance, Name, &Instance::getName,
                           &Instance::setName);
REFLECTION_PROPERTY_STRING(Instance, Type, &Instance::getClassName, NULL);
REFLECTION_PROPERTY_INSTANCE(Instance, Parent, &Instance::getParent,
                             &Instance::setParent);
REFLECTION_FUNCTION(Instance, GetChildren, &Instance::luaGetChildren);
REFLECTION_END_DESCRIBED();

Instance::Instance(DataModel* dataModel) {
  this->dataModel = dataModel;
  uuid = dataModel->newInstance(this);

  parent = "nil";
  name = "Instance";
}

Instance::~Instance() {
  setParent(NULL);

  for (auto childUUID : children) {
    Instance* instance = dataModel->getInstanceByUUID(childUUID);
    if (instance) {
      delete instance;
    }
  }

  dataModel->removeInstance(uuid);
}

Instance* Instance::getParent() {
  if (parent == "nil") return NULL;
  return dataModel->getInstanceByUUID(parent);
}

Instance* Instance::findFirstChildOfType(const char* type) {
  for (auto child : getChildren()) {
    if (child->isA(type)) {
      return child;
    }
  }
  return NULL;
}

Instance* Instance::findFirstChildOfName(const char* name) {
  for (auto child : getChildren()) {
    if (child->getName() == name) {
      return child;
    }
  }
  return NULL;
}

int Instance::luaGetChildren(lua_State* L) {
  Instance* instance =
      dynamic_cast<Instance*>(DescribedBridge::getDescribed(L, 1));
  auto children = instance->getChildren();
  lua_newtable(L);
  for (int i = 0; i < children.size(); i++) {
    lua_pushinteger(L, i);
    DescribedBridge::pushDescribed(L, children[i]);
    lua_settable(L, -3);
  }
  return 1;
}

void Instance::setParent(Instance* instance) {
  Instance* parent = dataModel->getInstanceByUUID(this->parent);
  if (parent) {
    parent->childRemoving.fire(parent, this);
    parent->children.erase(
        std::find(parent->children.begin(), parent->children.end(), uuid));
    Instance* oldParent = parent;
    this->parent = "nil";
    oldParent->childRemoved.fire(parent, this);
  }

  if (instance) {
    instance->childAdding.fire(instance, this);
    instance->children.push_back(uuid);
    parent = instance;
    this->parent = parent->getUUID();
    instance->childAdded.fire(instance, this);
  }
}

bool Instance::isDescendantOf(Instance* instance) {
  Instance* parent = getParent();
  while (parent != instance) {
    if (parent == NULL) return false;
    parent = parent->getParent();
  }
  return true;
}

std::vector<Instance*> Instance::getChildren() {
  std::vector<Instance*> instances;
  for (auto uuid : children) {
    Instance* i = dataModel->getInstanceByUUID(uuid);
    if (i) instances.push_back(i);
  }
  return instances;
}
};  // namespace freeblock
