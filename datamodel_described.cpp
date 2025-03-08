#include "datamodel_described.hpp"

#include "instance.hpp"
#include "reflection.hpp"
namespace freeblock {
INSTANCE_CTOR(DataModelDescribed, Service) {}

REFLECTION_BEGIN_DESCRIBED(DataModelDescribed);

REFLECTION_END_DESCRIBED();
}  // namespace freeblock
