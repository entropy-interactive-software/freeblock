#include "runservice.hpp"

#include <cstdint>

#include "BulletCollision/CollisionDispatch/btCollisionObject.h"
#include "BulletCollision/CollisionShapes/btStaticPlaneShape.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "LinearMath/btScalar.h"
#include "script_context.hpp"
#include "workspace.hpp"
namespace freeblock {
INSTANCE_CTOR(RunService, Service) {
  getDM()->getWorld()->stepped.listen([this] { _step(); });

  infinitePlaneShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
  btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, NULL,
                                                  infinitePlaneShape);
  infinitePlaneObject = new btRigidBody(rbInfo);
  infinitePlaneObject->setUserPointer(this);

  getDM()->getWorld()->getPhysicsWorld()->getWorld()->addRigidBody(
      infinitePlaneObject);

  setName("RunService");
}

RunService::~RunService() {
  getDM()->getWorld()->getPhysicsWorld()->getWorld()->removeRigidBody(
      infinitePlaneObject);
  delete infinitePlaneObject;
  delete infinitePlaneShape;
}

void RunService::start() {
  state = Running;
  updateSimulation();
}

void RunService::stop() {
  state = Stopped;
  updateSimulation();
}

void RunService::_step() {
  updateSimulation();

  if (state == Running) {
    getDM()->step();
    stepped.fire();
  }
}

void RunService::updateSimulation() {
  WorkspaceInstance* workspace =
      getDM()->getRoot()->getService<WorkspaceInstance>();
  rdm::World* world = getDM()->getWorld();
  {
    std::scoped_lock l(world->getPhysicsWorld()->mutex);
    world->getPhysicsWorld()->setStepSimulation(state == Running);
  }
}
};  // namespace freeblock
