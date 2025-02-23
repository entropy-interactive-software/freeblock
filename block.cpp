#include "block.hpp"

#include "BulletCollision/CollisionShapes/btBoxShape.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "LinearMath/btDefaultMotionState.h"
#include "LinearMath/btMotionState.h"
#include "LinearMath/btTransform.h"
#include "model.hpp"
namespace freeblock {
INSTANCE_CTOR(BlockInstance, PVInstance) {
  size = glm::vec3(2, 1, 4);
  shape = BlockInstance::Cuboid;
  color = rand() % 3;
  anchored = true;
  canCollide = true;

  collisionShape = new btBoxShape(rdm::BulletHelpers::toVector3(size / 2.f));
  collisionShape->setUserPointer(this);
  motionState = new btDefaultMotionState(btTransform::getIdentity());
  btVector3 inertia;
  collisionShape->calculateLocalInertia(1.0, inertia);
  btRigidBody::btRigidBodyConstructionInfo rbInfo(1.0, motionState,
                                                  collisionShape, inertia);
  rigidBody = new btRigidBody(rbInfo);
  rigidBody->setUserPointer(this);

  getDM()->getWorld()->getPhysicsWorld()->getWorld()->addRigidBody(rigidBody);
  getDM()->getWorld()->getPhysicsWorld()->physicsStepping.listen(
      [this] { physicsStep(); });
  physDirty = true;
}

void BlockInstance::physicsInit() {
  delete collisionShape;
  collisionShape = new btBoxShape(rdm::BulletHelpers::toVector3(size / 2.f));
  rigidBody->setCollisionShape(collisionShape);

  if (anchored) {
    rigidBody->setMassProps(0.0, btVector3(0.0, 0.0, 0.0));
  } else {
    btVector3 inertia;
    collisionShape->calculateLocalInertia(1.0, inertia);
    rigidBody->setMassProps(1.0, inertia);
  }

  btTransform transform;
  transform.setBasis(rdm::BulletHelpers::toMat3(getBasis()));
  transform.setOrigin(rdm::BulletHelpers::toVector3(getPosition()));
  rigidBody->setWorldTransform(transform);

  physDirty = false;
}

void BlockInstance::physicsStep() {
  btTransform transform;

  if (physDirty) {
    physicsInit();
  } else {
    if (!anchored && rigidBody->getActivationState()) {
      motionState->getWorldTransform(transform);
      setPosition(rdm::BulletHelpers::fromVector3(transform.getOrigin()));
      setBasis(rdm::BulletHelpers::fromMat3(transform.getBasis()));
      ModelInstance* model = dynamic_cast<ModelInstance*>(getParent());
      if (model) {
        model->setDirty(true);
      }
    }
  }
}
};  // namespace freeblock
