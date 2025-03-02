#pragma once
#include "BulletCollision/CollisionShapes/btCollisionShape.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "LinearMath/btMotionState.h"
#include "palette.hpp"
#include "pvinstance.hpp"
namespace freeblock {
class BlockInstance : public PVInstance {
  INSTANCE(BlockInstance, PVInstance);

  glm::vec3 size;

  btCollisionShape* collisionShape;
  btRigidBody* rigidBody;
  btMotionState* motionState;

  // color is pallete id
  BlockColor color;
  bool anchored;
  bool canCollide;

  bool physDirty;
  void physicsStep();

  rdm::ClosureId id;

 public:
  virtual ~BlockInstance();

  enum Shape {
    Cuboid,
    Sphere,
    Cylinder,
  };

  void physicsInit();

  void setSize(glm::vec3 s) {
    size = s;
    physDirty = true;
  }

  glm::vec3 getSize() { return size; }
  Shape getShape() { return shape; }
  BlockColor getColor() { return color; }

  bool getAnchored() { return anchored; }
  void setAnchored(bool b) {
    anchored = b;
    physDirty = true;
  }

  btRigidBody* getRigidBody() { return rigidBody; }

 private:
  Shape shape;
};
};  // namespace freeblock
