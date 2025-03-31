#include "runservice.hpp"

#include <cstdint>

#include "BulletCollision/CollisionDispatch/btCollisionObject.h"
#include "BulletCollision/CollisionShapes/btStaticPlaneShape.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "LinearMath/btScalar.h"
#include "datamodel_described.hpp"
#include "instance.hpp"
#include "reflection.hpp"
#include "reflection_props.hpp"
#include "script_api.hpp"
#include "script_context.hpp"
#include "workspace.hpp"
namespace freeblock {
INSTANCE_CTOR_SERVICE(RunService, Service) {
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

REFLECTION_BEGIN_DESCRIBED(RunService);
REFLECTION_FUNCTION(RunService, Start, &RunService::luaStart);
REFLECTION_END_DESCRIBED();

int RunService::luaStart(lua_State* L) {
  RunService* r =
      dynamic_cast<RunService*>(DescribedBridge::getDescribed(L, 1));
  r->start();
  return 0;
}

void RunService::start() {
  rdm::Log::printf(rdm::LOG_INFO, "Started");

  state = Running;
  getDM()->makeInstanceDirty(getUUID());

  updateSimulation();
}

void RunService::stop() {
  state = Stopped;
  getDM()->makeInstanceDirty(getUUID());

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
