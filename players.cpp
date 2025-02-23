#include "players.hpp"

#include "instance.hpp"
#include "player.hpp"
namespace freeblock {
INSTANCE_CTOR(PlayersService, Service) {}

void PlayersService::createLocalPlayer() {
  PlayerInstance* player = createChild<PlayerInstance>();
  localPlayer = player->getUUID();
  player->setName("TestPlayer");
  player->spawnCharacter();
}
};  // namespace freeblock
