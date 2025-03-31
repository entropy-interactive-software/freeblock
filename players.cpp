#include "players.hpp"

#include "instance.hpp"
#include "player.hpp"
namespace freeblock {
INSTANCE_CTOR_SERVICE(PlayersService, Service) {}

void PlayersService::createLocalPlayer() {
  PlayerInstance* player = createChild<PlayerInstance>();
  localPlayer = player->getUUID();
  player->setName("TestPlayer");
  player->spawnCharacter();
}

void PlayersService::step() {
  if (getDM()->isServer()) {
    auto peers = getDM()->getWorld()->getNetworkManager()->getPeers();
    for (auto& [id, peer] : peers) {
      if (!peer.playerEntity) continue;
      NetworkPlayerEntity* npe =
          dynamic_cast<NetworkPlayerEntity*>(peer.playerEntity);
      PlayerInstance* player = NULL;
      if (Instance* player_instance =
              getDM()->getInstanceByUUID(npe->getUUID())) {
        player = dynamic_cast<PlayerInstance*>(player_instance);
      } else {
        player = createChild<PlayerInstance>();
        npe->setUUID(player->getUUID());
        player->setName(npe->displayName.get().c_str());
        // player->spawnCharacter();
      }

      if (!player) {
      }
    }
  } else if (getDM()->isClient()) {
    auto localPeer = getDM()->getWorld()->getNetworkManager()->getLocalPeer();
    NetworkPlayerEntity* npe =
        dynamic_cast<NetworkPlayerEntity*>(localPeer.playerEntity);
    if (npe) {
    }
  }
}
};  // namespace freeblock
