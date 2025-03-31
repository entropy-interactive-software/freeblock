#pragma once
#include "datamodel.hpp"
#include "player.hpp"
#include "service.hpp"
namespace freeblock {
class PlayersService : public Service {
  INSTANCE(PlayersService, Service)

  InstanceUUID localPlayer;

 public:
  void createLocalPlayer();

  PlayerInstance* getLocalPlayer() {
    return getDM()->getInstanceByUUID<PlayerInstance>(localPlayer);
  }

  virtual void step();
};
};  // namespace freeblock
