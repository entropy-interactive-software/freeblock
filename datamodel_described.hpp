#pragma once
#include "instance.hpp"
#include "reflection.hpp"
#include "service.hpp"
namespace freeblock {
class DataModelDescribed : public Service {
  DESCRIBED;
  INSTANCE(DataModelDescribed, Service);

 public:
};
};  // namespace freeblock
