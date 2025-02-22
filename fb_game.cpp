#include "fb_game.hpp"

#include "datamodel.hpp"
#include "pipeline.hpp"
#include "script.hpp"
#include "script_context.hpp"
#include "world.hpp"
namespace freeblock {
void Game::initialize() { startClient(); }
void Game::initializeClient() {
  getWorld()->setTitle("Freeblock");
  getWorld()->setUser(new DataModel());
  DataModel* dm = (DataModel*)getWorld()->getUser();

  renderPipeline.reset(new Pipeline(getGfxEngine(), dm));
}
}  // namespace freeblock
