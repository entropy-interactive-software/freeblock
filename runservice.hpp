#pragma once
#include "service.hpp"
namespace freeblock {
class RunService : public Service {
  INSTANCE(RunService, Service);

 public:
  rdm::Signal<> stepped;
};
};  // namespace freeblock
