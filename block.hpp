#pragma once
#include "palette.hpp"
#include "pvinstance.hpp"
namespace freeblock {
class BlockInstance : public PVInstance {
  INSTANCE(BlockInstance, PVInstance);

  glm::vec3 size;

  // color is pallete id
  BlockColor color;

 public:
  enum Shape {
    Cuboid,
    Sphere,
    Cylinder,
  };

  glm::vec3 getSize() { return size; }
  Shape getShape() { return shape; }
  BlockColor getColor() { return color; }

 private:
  Shape shape;
};
};  // namespace freeblock
