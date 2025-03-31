#pragma once
#include <glm/glm.hpp>

#include "instance.hpp"
namespace freeblock {
class PVInstance : public Instance {
  DESCRIBED;
  INSTANCE(PVInstance, Instance);

  glm::vec3 position;
  glm::mat3 basis;
  bool dirtyValues;

 public:
  glm::vec3 getPosition() { return position; }
  glm::mat3 getBasis() { return basis; }

  bool& getDirtyValues() { return dirtyValues; }

  void setPosition(glm::vec3 v) {
    position = v;
    dirtyValues = true;
  }
  void setBasis(glm::mat3 m) {
    basis = m;
    dirtyValues = true;
  }
};
}  // namespace freeblock
