#include "fb_game.hpp"

#include "block.hpp"
#include "datamodel.hpp"
#include "pipeline.hpp"
#include "script.hpp"
#include "script_context.hpp"
#include "workspace.hpp"
#include "world.hpp"
namespace freeblock {
void Game::initialize() { startClient(); }
void Game::initializeClient() {
  getWorld()->setTitle("Freeblock");
  getWorld()->setUser(new DataModel(getWorld()));
  DataModel* dm = (DataModel*)getWorld()->getUser();
  WorkspaceInstance* workspace = dm->getRoot()->getService<WorkspaceInstance>();
  BlockInstance* block = workspace->createChild<BlockInstance>();

  renderPipeline.reset(new Pipeline(getGfxEngine(), dm));
}
}  // namespace freeblock
