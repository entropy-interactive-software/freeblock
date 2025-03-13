#include "pvinstance.hpp"

#include "block.hpp"
#include "instance.hpp"
#include "reflection.hpp"
#include "reflection_props.hpp"
namespace freeblock {
INSTANCE_CTOR(PVInstance, Instance) {
  position = glm::vec3(0.0);
  basis = glm::mat3(1);
}

REFLECTION_BEGIN_DESCRIBED(PVInstance);
REFLECTION_PROPERTY_VEC3(PVInstance, Position, &PVInstance::getPosition,
                         &PVInstance::setPosition);
REFLECTION_PROPERTY_MAT3(PVInstance, Basis, &PVInstance::getBasis,
                         &PVInstance::setBasis);
REFLECTION_END_DESCRIBED();
}  // namespace freeblock
