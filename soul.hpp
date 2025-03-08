#pragma once
#include "instance.hpp"
namespace freeblock {
class SoulInstance : public Instance {
  INSTANCE(SoulInstance, Instance);

 public:
  virtual void step();
};
}  // namespace freeblock
