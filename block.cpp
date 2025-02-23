#include "block.hpp"
namespace freeblock {
INSTANCE_CTOR(BlockInstance, PVInstance) {
  size = glm::vec3(2, 1, 4);
  shape = BlockInstance::Cuboid;
  color = 0;
}
};  // namespace freeblock
