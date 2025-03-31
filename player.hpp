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
  InstanceUUID uuid;

 public:
  NetworkPlayerEntity(rdm::network::NetworkManager* manager,
                      rdm::network::EntityId id);

  void setUUID(InstanceUUID uuid) {
    this->uuid = uuid;
    getManager()->addPendingUpdate(getEntityId());
  }
  InstanceUUID getUUID() { return uuid; }

  virtual void serialize(rdm::network::BitStream& stream);
  virtual void deserialize(rdm::network::BitStream& stream);

  virtual const char* getTypeName() { return "player_tracked"; };
};
};  // namespace freeblock
