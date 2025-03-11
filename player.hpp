#pragma once
#include "datamodel.hpp"
#include "instance.hpp"
#include "network/player.hpp"
#include "soul.hpp"
namespace freeblock {
class PlayerInstance : public Instance {
  DESCRIBED;
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

  virtual const char* getTypeName() { return "player_tracked"; };
};
};  // namespace freeblock
