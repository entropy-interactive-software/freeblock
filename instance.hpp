#pragma once
#include <signal.hpp>
#include <string>
#include <type_traits>

#include "datamodel.hpp"
#include "reflection.hpp"
namespace freeblock {
#define INSTANCE(N, P)                                    \
 public:                                                  \
  typedef P Super;                                        \
  typedef N Self;                                         \
  virtual std::string getClassName() const { return #N; } \
  template <typename T>                                   \
  static bool isA(Instance* instance) {                   \
    if (dynamic_cast<T*>(instance)) {                     \
      return true;                                        \
    } else {                                              \
      return Instance::isA<Super>(instance);              \
    }                                                     \
  }                                                       \
  virtual bool isA(const char* type) const {              \
    if (getClassName() == type) {                         \
      return true;                                        \
    } else {                                              \
      return Super::isA(type);                            \
    }                                                     \
  }                                                       \
  N(DataModel* dm);                                       \
                                                          \
 private:
#define INSTANCE_CTOR(N, P) N::N(DataModel* dm) : P(dm)
class Instance : public reflection::Described {
  DataModel* dataModel;
  InstanceUUID parent;
  std::vector<InstanceUUID> children;
  std::string name;
  InstanceUUID uuid;

 public:
  Instance(DataModel* dataModel);
  virtual ~Instance();

  virtual std::string getClassName() const { return "Instance"; }

  // this, child
  rdm::Signal<Instance*, Instance*> childAdding;
  // this, child
  rdm::Signal<Instance*, Instance*> childAdded;

  // this, child
  rdm::Signal<Instance*, Instance*> childRemoving;
  // this, child
  rdm::Signal<Instance*, Instance*> childRemoved;

  rdm::Signal<Instance*, Instance*> descendantAdding;
  rdm::Signal<Instance*, Instance*> descendantRemoving;

  void setParent(Instance* instance);
  void addChild(Instance* instance) { instance->setParent(this); };
  template <typename T>
  T* createChild() {
    static_assert(std::is_base_of<Instance, T>::value,
                  "T must inherit from Instance");
    T* ins = new T(getDM());
    addChild(ins);
    return ins;
  };

  Instance* findFirstChildOfType(const char* type);

  template <typename T>
  T* findFirstChildOfType() {
    static_assert(std::is_base_of<Instance, T>::value,
                  "T must inherit from Instance");
    for (auto child : getChildren()) {
      if (Instance::isA<T>(child)) {
        return (T*)child;
      }
    }
    return NULL;
  }

  virtual bool isA(const char* type) const { return (getClassName() == type); }
  template <typename T>
  static bool isA(Instance* instance) {
    if (dynamic_cast<T*>(instance)) {
      return true;
    } else {
      return false;
    }
  }

  InstanceUUID getUUID() { return uuid; }
  Instance* getParent();
  DataModel* getDM() { return dataModel; }
  std::vector<Instance*> getChildren();
  std::string getName() { return name; }

  template <typename T>
  T* getService() {
    static_assert(std::is_base_of<Instance, T>::value,
                  "T must inherit from Instance");
    Instance* root = dataModel->getRoot();
    T* service = root->findFirstChildOfType<T>();
    if (service) {
      return service;
    } else {
      service = new T(dataModel);
      root->addChild(service);
      return service;
    }
  }
};
};  // namespace freeblock
