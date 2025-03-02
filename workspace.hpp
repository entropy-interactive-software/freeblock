#pragma once
#include "model.hpp"
namespace freeblock {
class WorkspaceInstance : public ModelInstance {
  INSTANCE(WorkspaceInstance, ModelInstance);

  bool infinitePlane;

 public:
  void setInfinitePlane(bool b) { infinitePlane = b; };
  bool getInfinitePlane() { return infinitePlane; }
};
};  // namespace freeblock
