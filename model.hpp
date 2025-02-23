#pragma once
#include "instance.hpp"
namespace freeblock {
class ModelInstance : public Instance {
  INSTANCE(ModelInstance, Instance);

  bool dirty;

 public:
  void setDirty(bool d) { dirty = d; };
  bool getDirty() { return dirty; }
};
};  // namespace freeblock
