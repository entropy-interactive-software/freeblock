#pragma once
#include <glm/glm.hpp>

#include "instance.hpp"
namespace freeblock {
class PVInstance : public Instance {
  DESCRIBED;
  INSTANCE(PVInstance, Instance);

  glm::vec3 position;
  glm::mat3 basis;

 public:
  glm::vec3 getPosition() { return position; }
  glm::mat3 getBasis() { return basis; }

  void setPosition(glm::vec3 v) { position = v; }
  void setBasis(glm::mat3 m) { basis = m; }
};
}  // namespace freeblock
