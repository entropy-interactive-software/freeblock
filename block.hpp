#pragma once
#include "pvinstance.hpp"
namespace freeblock {
class BlockInstance : public PVInstance {
  INSTANCE(BlockInstance, PVInstance);

  glm::vec3 size;

 public:
  enum Shape {
    Cuboid,
    Sphere,
    Cylinder,
  };

  glm::vec3 getSize() { return size; }
  Shape getShape() { return shape; }

 private:
  Shape shape;
};
};  // namespace freeblock
