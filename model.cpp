#include "model.hpp"

#include "block.hpp"
#include "instance.hpp"
#include "pvinstance.hpp"
namespace freeblock {
INSTANCE_CTOR(ModelInstance, Instance) {}

void ModelInstance::moveTo(glm::vec3 position) {
  BlockInstance* primary = getPrimaryBlock();
  glm::vec3 primaryPosition = primary->getPosition();
  if (!primary) return;
  for (auto child : getChildren()) {
    PVInstance* pv = dynamic_cast<PVInstance*>(child);
    if (!pv) continue;
    if (pv == primary) {
      pv->setPosition(position);
    } else {
      glm::vec3 rel = pv->getPosition() - primaryPosition;
      pv->setPosition(rel + position);
    }
  }
}
};  // namespace freeblock
