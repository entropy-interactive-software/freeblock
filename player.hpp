#pragma once
#include "datamodel.hpp"
#include "instance.hpp"
#include "soul.hpp"
namespace freeblock {
class PlayerInstance : public Instance {
  INSTANCE(PlayerInstance, Instance);

  InstanceUUID character;

 public:
  SoulInstance* getCharacter() {
    return getDM()->getInstanceByUUID<SoulInstance>(character);
  }

  void spawnCharacter();
};
};  // namespace freeblock
