#pragma once
#include "datamodel.hpp"
#include "instance.hpp"
#include "network/player.hpp"
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

class NetworkPlayerEntity : public rdm::network::Player {
 public:
  NetworkPlayerEntity(rdm::network::NetworkManager* manager,
                      rdm::network::EntityId id);
};
};  // namespace freeblock
