#pragma once
#include "pvinstance.hpp"
namespace freeblock {
class BlockInstance : public PVInstance {
  INSTANCE(BlockInstance, PVInstance);

  glm::vec3 size;

 public:
  glm::vec3 getSize() { return size; }
};
};  // namespace freeblock
