#pragma once
#include "model.hpp"
#include "reflection.hpp"
namespace freeblock {
class WorkspaceInstance : public ModelInstance {
  DESCRIBED;
  INSTANCE(WorkspaceInstance, ModelInstance);

  bool infinitePlane;

 public:
  void setInfinitePlane(bool b) {
    infinitePlane = b;
    getDM()->makeInstanceDirty(getUUID());
  };
  bool getInfinitePlane() { return infinitePlane; }
};
};  // namespace freeblock
