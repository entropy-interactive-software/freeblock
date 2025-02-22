#include "pvinstance.hpp"

#include "block.hpp"
#include "instance.hpp"
namespace freeblock {
INSTANCE_CTOR(PVInstance, Instance) {
  position = glm::vec3(0.0);
  basis = glm::mat3(1);
}
}  // namespace freeblock
