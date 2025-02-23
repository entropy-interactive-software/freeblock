#include "fb_game.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>

#include "block.hpp"
#include "datamodel.hpp"
#include "input.hpp"
#include "model.hpp"
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

  r = glm::mat4(1);

  renderPipeline.reset(new Pipeline(getGfxEngine(), dm));

  getGfxEngine()->renderStepped.listen([this, dm] {
    rdm::gfx::Camera& cam = getGfxEngine()->getCamera();
    cam.setUp(glm::vec3(0, 1, 0));

    if (PlayerInstance* player =
            dm->getRoot()->getService<PlayersService>()->getLocalPlayer()) {
      if (SoulInstance* soul = player->getCharacter()) {
        ModelInstance* model = dynamic_cast<ModelInstance*>(soul->getParent());
        if (model) {
          BlockInstance* head =
              dynamic_cast<BlockInstance*>(model->findFirstChildOfName("Head"));
          if (head) {
            cam.setTarget(head->getPosition());

            if (rdm::Input::singleton()->isMouseButtonDown(3)) {
              glm::vec2 delta = rdm::Input::singleton()->getMouseDelta();
              if (delta.x < 0.f) {
                r = glm::rotate(r, glm::radians(-abs(delta.x)),
                                glm::vec3(0, 1, 0));
              } else if (delta.x > 0.f) {
                r = glm::rotate(r, glm::radians(abs(delta.x)),
                                glm::vec3(0, 1, 0));
              }

              if (delta.y > 0.f) {
                r = glm::rotate(r, glm::radians(abs(delta.y)),
                                glm::vec3(1, 0, 0));
              } else if (delta.y < 0.f) {
                r = glm::rotate(r, glm::radians(-abs(delta.y)),
                                glm::vec3(1, 0, 0));
              }
            }

            cam.setPosition((glm::mat3(r) * glm::vec3(5, 5, 5)) +
                            head->getPosition());
          }
        }
      }
    } else {
      cam.setTarget(glm::vec3(0, 0, 0));
      cam.setPosition(glm::vec3(sinf(getGfxEngine()->getTime()) * 200.f, 200,
                                cosf(getGfxEngine()->getTime()) * 200.f));
    }
  });

  dm->getRoot()->getService<PlayersService>()->createLocalPlayer();
}
}  // namespace freeblock
