#include "fb_game.hpp"

#include "block.hpp"
#include "datamodel.hpp"
#include "pipeline.hpp"
#include "script.hpp"
#include "script_context.hpp"
#include "settings.hpp"
#include "workspace.hpp"
#include "world.hpp"
namespace freeblock {
void Game::initialize() {
  rdm::WorldConstructorSettings& settings = getWorldConstructorSettings();
  settings.network = true;

  startClient();
}

DataModel* Game::createDM(rdm::World* world) {
  world->setTitle("Freeblock");
  world->setUser(new DataModel(getWorld()));
  return (DataModel*)world->getUser();
}

void Game::initializeServer() { createDM(getServerWorld()); }

void Game::initializeClient() {
  DataModel* dm = createDM(getWorld());

  WorkspaceInstance* workspace = dm->getRoot()->getService<WorkspaceInstance>();
  BlockInstance* block = workspace->createChild<BlockInstance>();

  renderPipeline.reset(new Pipeline(getGfxEngine(), dm));

  getGfxEngine()->renderStepped.listen([this] {
    rdm::gfx::Camera& cam = getGfxEngine()->getCamera();
    cam.setUp(glm::vec3(0, 1, 0));
    cam.setTarget(glm::vec3(0, 0, 0));
    cam.setPosition(glm::vec3(sinf(getGfxEngine()->getTime()) * 200.f, 200,
                              cosf(getGfxEngine()->getTime()) * 200.f));
  });
}
}  // namespace freeblock
