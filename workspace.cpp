#include "workspace.hpp"

#include "instance.hpp"
#include "reflection.hpp"
#include "reflection_props.hpp"
namespace freeblock {
INSTANCE_CTOR_REPLICATABLE(WorkspaceInstance, ModelInstance) {
  setName("Workspace");
}

REFLECTION_BEGIN_DESCRIBED(WorkspaceInstance);
REFLECTION_PROPERTY_BOOL(WorkspaceInstance, InfinitePlane,
                         &WorkspaceInstance::getInfinitePlane,
                         &WorkspaceInstance::setInfinitePlane);
REFLECTION_END_DESCRIBED();
};  // namespace freeblock
