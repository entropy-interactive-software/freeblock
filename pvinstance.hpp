#pragma once
#include <glm/glm.hpp>

#include "instance.hpp"
namespace freeblock {
class PVInstance : public Instance {
  INSTANCE(PVInstance, Instance);

  glm::vec3 position;
  glm::mat3 basis;

 public:
  glm::vec3 getPosition() { return position; }
  glm::mat3 getBasis() { return basis; }
};
}  // namespace freeblock
