#include "soul.hpp"

#include "block.hpp"
#include "instance.hpp"
namespace freeblock {
INSTANCE_CTOR_CREATABLE(SoulInstance, Instance) {}

void SoulInstance::step() {
  Instance* _body = getParent()->findFirstChildOfName("Body");
  if (BlockInstance* body = dynamic_cast<BlockInstance*>(_body)) {
    body->getRigidBody()->setActivationState(1);
  }
}
};  // namespace freeblock
