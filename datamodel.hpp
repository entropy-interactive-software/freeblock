#pragma once
#include <map>
#include <memory>
#include <string>
#include <unordered_map>

namespace freeblock {
class Instance;

typedef std::string InstanceUUID;

class DataModel {
  Instance* root;
  std::unordered_map<InstanceUUID, Instance*> instances;

 public:
  DataModel();
  ~DataModel();

  Instance* getInstanceByUUID(InstanceUUID uuid);
  InstanceUUID newInstance(Instance* instance);
  Instance* getRoot() { return root; }
};
};  // namespace freeblock
