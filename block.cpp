#include "block.hpp"

#include "BulletCollision/CollisionShapes/btBoxShape.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "LinearMath/btDefaultMotionState.h"
#include "LinearMath/btMotionState.h"
#include "LinearMath/btTransform.h"
#include "model.hpp"
#include "reflection.hpp"
#include "reflection_props.hpp"
namespace freeblock {
INSTANCE_CTOR_CREATABLE(BlockInstance, PVInstance) {
  size = glm::vec3(2, 1, 4);
  shape = BlockInstance::Cuboid;
  color = rand() % 3;
  anchored = true;
  canCollide = true;

  {
    std::scoped_lock l(getDM()->getWorld()->getPhysicsWorld()->mutex);
    collisionShape = new btBoxShape(rdm::BulletHelpers::toVector3(size / 2.f));
    collisionShape->setUserPointer(this);
    motionState = new btDefaultMotionState(btTransform::getIdentity());
    btVector3 inertia;
    collisionShape->calculateLocalInertia(size.length(), inertia);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(size.length(), motionState,
                                                    collisionShape, inertia);
    rigidBody = new btRigidBody(rbInfo);
    rigidBody->setUserPointer(this);
  }

  getDM()->getWorld()->getPhysicsWorld()->getWorld()->addRigidBody(rigidBody);
  id = getDM()->getWorld()->getPhysicsWorld()->physicsStepping.listen(
      [this] { physicsStep(); });
  physDirty = true;
}

REFLECTION_BEGIN_DESCRIBED(BlockInstance);
REFLECTION_PROPERTY_VEC3(BlockInstance, Size, &BlockInstance::getSize,
                         &BlockInstance::setSize);
REFLECTION_PROPERTY_BOOL(BlockInstance, Anchored, &BlockInstance::getAnchored,
                         &BlockInstance::setAnchored);
REFLECTION_PROPERTY_INT(BlockInstance, BlockColor, &BlockInstance::getColor,
                        &BlockInstance::setColor);
REFLECTION_END_DESCRIBED();

BlockInstance::~BlockInstance() {
  getDM()->getWorld()->getPhysicsWorld()->getWorld()->removeRigidBody(
      rigidBody);
  delete rigidBody;
  delete motionState;
  delete collisionShape;
  getDM()->getWorld()->getPhysicsWorld()->physicsStepping.removeListener(id);
}

void BlockInstance::physicsInit() {
  std::scoped_lock l(getDM()->getWorld()->getPhysicsWorld()->mutex);

  delete collisionShape;
  collisionShape = new btBoxShape(rdm::BulletHelpers::toVector3(size / 2.f));
  rigidBody->setCollisionShape(collisionShape);

  if (anchored) {
    rigidBody->setMassProps(0.0, btVector3(0.0, 0.0, 0.0));
  } else {
    btVector3 inertia;
    collisionShape->calculateLocalInertia(size.length(), inertia);
    rigidBody->setMassProps(size.length(), inertia);
  }

  btTransform transform;
  transform.setBasis(rdm::BulletHelpers::toMat3(getBasis()));
  transform.setOrigin(rdm::BulletHelpers::toVector3(getPosition()));
  rigidBody->setWorldTransform(transform);

  if (!anchored) {
    rigidBody->activate(true);
  }

  physDirty = false;
}

void BlockInstance::physicsStep() {
  btTransform transform;

  if (physDirty) {
    physicsInit();
    if (ModelInstance* parent = dynamic_cast<ModelInstance*>(getParent())) {
      parent->setDirty(true);
    }
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
