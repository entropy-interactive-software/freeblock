#pragma once
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "world.hpp"

namespace freeblock {
class Instance;

typedef std::string InstanceUUID;
class DataModelDescribed;

class DataModel {
  friend class DataModelTrackingEntity;

  DataModelDescribed* root;
  rdm::World* world;
  std::unordered_map<InstanceUUID, Instance*> instances;

  std::mutex dirtyInstanceLock;
  std::vector<InstanceUUID> dirtyInstances;

  std::mutex writeMutex;

 public:
  DataModel(rdm::World* world);
  ~DataModel();

  bool isServer();
  bool isClient();

  void step();
  void gc();

  void loadLegacyMap(const char* path);

  rdm::World* getWorld() { return world; }

  Instance* getInstanceByUUID(InstanceUUID uuid);
  template <typename T>
  T* getInstanceByUUID(InstanceUUID uuid) {
    Instance* i = getInstanceByUUID(uuid);
    if (!i) return NULL;
    return dynamic_cast<T*>(i);
  }

  void removeInstance(InstanceUUID uuid);
  void makeInstanceDirty(InstanceUUID uuid);
  std::vector<InstanceUUID> getDirtyInstances() {
    std::scoped_lock lock(dirtyInstanceLock);
    if (!dirtyInstances.size()) return {};
    auto copy = dirtyInstances;
    dirtyInstances.clear();
    return copy;
  }

  static std::string generateUUID();

  InstanceUUID newInstance(Instance* instance);
  void newInstanceTracked(Instance* instance, InstanceUUID uuid);
  Instance* getRoot() { return (Instance*)root; }

  void setInstanceUUID(InstanceUUID old, InstanceUUID newu);

  void create(const char* name);

  std::mutex& getMutex() { return writeMutex; }
};
};  // namespace freeblock
