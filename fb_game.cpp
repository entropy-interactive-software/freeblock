#include "fb_game.hpp"

#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "block.hpp"
#include "datamodel.hpp"
#include "datamodel_entity.hpp"
#include "http.hpp"
#include "input.hpp"
#include "model.hpp"
#include "network/network.hpp"
#include "pipeline.hpp"
#include "player.hpp"
#include "players.hpp"
#include "script.hpp"
#include "script_context.hpp"
#include "settings.hpp"
#include "soul.hpp"
#include "workspace.hpp"
#include "world.hpp"
namespace freeblock {
void Game::initialize() {
  rdm::WorldConstructorSettings& settings = getWorldConstructorSettings();
  settings.network = true;
  settings.physics = true;

  Http::globalInit();  // init

  Http* http = new Http("https://example.com/");
  http->start();

  if (editor) {
    startClient();
    startServer();
  } else {
    if (!rdm::Settings::singleton()->getHintDs()) {
      startClient();
    } else {
      startServer();
    }
  }
}

void Game::addEntityConstructors(rdm::network::NetworkManager* manager) {
  manager->setPassword("FREE_BLOCK_IS_BETTER");
  manager->registerConstructor(net::EntityConstructor<DataModelTrackingEntity>,
                               "datamodel");
  manager->registerConstructor(net::EntityConstructor<NetworkPlayerEntity>,
                               "player_tracked");
  manager->setPlayerType("player_tracked");
}

void Game::initializeServer() {
  addEntityConstructors(worldServer->getNetworkManager());
  worldServer->getNetworkManager()->start();

  worldServer->getNetworkManager()->instantiate("datamodel");
}

void Game::initializeClient() {
  addEntityConstructors(world->getNetworkManager());
  world->getNetworkManager()->connect("127.0.0.1");

  /*
  Instance* root = dm->getRoot();
  ScriptInstance* script = root->createChild<ScriptInstance>();
  script->setLinkedSource("content/scripts/test.lua");
  */
  // dm->getRoot()->getService<PlayersService>()->createLocalPlayer();
}
}  // namespace freeblock
