#pragma once
#include "instance.hpp"
namespace freeblock {
class MeshInstance : public Instance {
  DESCRIBED;
  INSTANCE(MeshInstance, Instance);

  std::string meshPath;

 public:
  std::string getMeshPath() { return meshPath; };
  void setMeshPath(std::string mesh) { meshPath = mesh; };
};
};  // namespace freeblock
