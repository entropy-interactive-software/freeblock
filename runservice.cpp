#include "runservice.hpp"

#include "script_context.hpp"
namespace freeblock {
INSTANCE_CTOR(RunService, Service) {
  getDM()->getWorld()->stepped.listen([this] { step(); });
}

void RunService::start() {
  state = Running;
  updateSimulation();
}

void RunService::stop() {
  state = Stopped;
  updateSimulation();
}

void RunService::step() {
  updateSimulation();

  if (state == Running) {
    ScriptContext* context = getDM()->getRoot()->getService<ScriptContext>();
    context->step();

    stepped.fire();
  }
}

void RunService::updateSimulation() {
  rdm::World* world = getDM()->getWorld();
  {
    std::scoped_lock l(world->getPhysicsWorld()->mutex);
    world->getPhysicsWorld()->setStepSimulation(state == Running);
  }
}
};  // namespace freeblock
