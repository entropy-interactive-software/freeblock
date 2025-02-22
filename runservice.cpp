#include "runservice.hpp"
namespace freeblock {
INSTANCE_CTOR(RunService, Service) {
  getDM()->getWorld()->stepped.listen([this] { stepped.fire(); });
}
};  // namespace freeblock
