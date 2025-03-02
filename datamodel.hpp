#pragma once
#include <map>
#include <memory>
#include <string>
#include <unordered_map>

#include "network/entity.hpp"
#include "world.hpp"

namespace freeblock {
class Instance;

typedef std::string InstanceUUID;

class DataModel {
  Instance* root;
  rdm::World* world;
  std::unordered_map<InstanceUUID, Instance*> instances;

 public:
  DataModel(rdm::World* world);
  ~DataModel();

  bool isServer();
  bool isClient();

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

  InstanceUUID newInstance(Instance* instance);
  Instance* getRoot() { return root; }

  void create(const char* name);
};

class DataModelTrackingEntity {
 public:
  DataModelTrackingEntity(rdm::network::NetworkManager* manager,
                          rdm::network::EntityId id);
};
};  // namespace freeblock
