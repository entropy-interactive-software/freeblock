#include "mesh.hpp"

#include "reflection.hpp"
#include "reflection_props.hpp"
namespace freeblock {
INSTANCE_CTOR(MeshInstance, Instance) {}

REFLECTION_BEGIN_DESCRIBED(MeshInstance);
REFLECTION_PROPERTY_STRING(MeshInstance, Content, &MeshInstance::getMeshPath,
                           &MeshInstance::setMeshPath);
REFLECTION_END_DESCRIBED();
}  // namespace freeblock
