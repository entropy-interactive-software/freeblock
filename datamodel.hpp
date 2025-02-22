#pragma once
#include <map>
#include <memory>
#include <string>
#include <unordered_map>

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

  rdm::World* getWorld() { return world; }

  Instance* getInstanceByUUID(InstanceUUID uuid);
  InstanceUUID newInstance(Instance* instance);
  Instance* getRoot() { return root; }
};
};  // namespace freeblock
