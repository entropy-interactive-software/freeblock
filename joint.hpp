#pragma once
#include "LinearMath/btVector3.h"
#include "block.hpp"
#include "datamodel.hpp"
#include "instance.hpp"
#include "model.hpp"
#include "service.hpp"
namespace freeblock {
class WeldInstance : public Instance {
  INSTANCE(WeldInstance, Instance);

  InstanceUUID a;
  btVector3 pointA;
  InstanceUUID b;
  btVector3 pointB;

 public:
  void setA(BlockInstance* a) { this->a = INSTANCE_TOUUID(a); };
  void setB(BlockInstance* b) { this->b = INSTANCE_TOUUID(b); };
  void setPointA(btVector3 a) { this->pointA = a; }
  void setPointB(btVector3 b) { this->pointB = b; }

  void link();
};

class JointService : public Service {
  INSTANCE(JointService, Service);

 public:
  void buildJoints(ModelInstance* model);
};
};  // namespace freeblock
