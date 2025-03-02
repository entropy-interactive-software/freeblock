#include "fb_game.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>

#include "BulletDynamics/Dynamics/btRigidBody.h"
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
  settings.physics = true;

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

  r1 = glm::mat4(1);
  r2 = glm::mat4(1);

  getGfxEngine()->initialized.listen([this] {
    rdm::gfx::Engine* engine = getGfxEngine();
    std::vector<void*> cubemap_textures = {
        engine->getTextureCache()
            ->getOrLoad2d("dat5/baseq3/textures/skies/null_plainsky512_rt.jpg",
                          true)
            .value()
            .first.data,
        engine->getTextureCache()
            ->getOrLoad2d("dat5/baseq3/textures/skies/null_plainsky512_lf.jpg",
                          true)
            .value()
            .first.data,
        engine->getTextureCache()
            ->getOrLoad2d("dat5/baseq3/textures/skies/null_plainsky512_dn.jpg",
                          true)
            .value()
            .first.data,
        engine->getTextureCache()
            ->getOrLoad2d("dat5/baseq3/textures/skies/null_plainsky512_up.jpg",
                          true)
            .value()
            .first.data,
        engine->getTextureCache()
            ->getOrLoad2d("dat5/baseq3/textures/skies/null_plainsky512_bk.jpg",
                          true)
            .value()
            .first.data,
        engine->getTextureCache()
            ->getOrLoad2d("dat5/baseq3/textures/skies/null_plainsky512_ft.jpg",
                          true)
            .value()
            .first.data,
    };
    std::unique_ptr<rdm::gfx::BaseTexture> skybox =
        engine->getDevice()->createTexture();
    skybox->uploadCubeMap(
        engine->getTextureCache()
            ->getOrLoad2d("dat5/baseq3/textures/skies/null_plainsky512_lf.jpg")
            .value()
            .first.width,
        engine->getTextureCache()
            ->getOrLoad2d("dat5/baseq3/textures/skies/null_plainsky512_lf.jpg")
            .value()
            .first.height,
        cubemap_textures);
    rdm::gfx::TextureCache::Info info;
    info.channels = 3;
    info.data = NULL;
    info.width = 256;
    info.height = 256;
    m_skybox = skybox.get();
    engine->getTextureCache()->cacheExistingTexture("null_plainsky512", skybox,
                                                    info);
  });

  renderPipeline.reset(new Pipeline(getGfxEngine(), dm));

  getGfxEngine()->renderStepped.listen([this, dm] {
    rdm::gfx::Camera& cam = getGfxEngine()->getCamera();
    cam.setUp(glm::vec3(0, 1, 0));

    bool isGhost = false;
    if (PlayerInstance* player =
            dm->getRoot()->getService<PlayersService>()->getLocalPlayer()) {
      if (SoulInstance* soul = player->getCharacter()) {
        ModelInstance* model = dynamic_cast<ModelInstance*>(soul->getParent());
        if (model) {
          BlockInstance* head =
              dynamic_cast<BlockInstance*>(model->findFirstChildOfName("Body"));
          if (head) {
            cam.setTarget(head->getPosition());
          }
        }
      }
    } else {
      cam.setTarget(glm::vec3(0, 0, 0));
      isGhost = true;
    }

    if (rdm::Input::singleton()->isMouseButtonDown(3)) {
      glm::vec2 delta = rdm::Input::singleton()->getMouseDelta();
      if (delta.x < 0.f) {
        r1 = glm::rotate(r1, glm::radians(-abs(delta.x)), glm::vec3(0, 1, 0));
      } else if (delta.x > 0.f) {
        r1 = glm::rotate(r1, glm::radians(abs(delta.x)), glm::vec3(0, 1, 0));
      }

      if (delta.y > 0.f) {
        r2 = glm::rotate(r2, glm::radians(abs(delta.y)), glm::vec3(1, 0, 0));
      } else if (delta.y < 0.f) {
        r2 = glm::rotate(r2, glm::radians(-abs(delta.y)), glm::vec3(1, 0, 0));
      }
    }

    {
      getGfxEngine()->getDevice()->setDepthState(
          rdm::gfx::BaseDevice::Disabled);
      getGfxEngine()->getDevice()->setCullState(rdm::gfx::BaseDevice::None);
      auto mt = getGfxEngine()->getMaterialCache()->getOrLoad("Skybox").value();
      rdm::gfx::BaseProgram* bp =
          mt->prepareDevice(getGfxEngine()->getDevice(), 0);
      glm::mat4 model(1);
      model = glm::translate(model, cam.getPosition());
      model = glm::scale(model, glm::vec3(20, 20, 20));
      bp->setParameter("model", rdm::gfx::DtMat4,
                       rdm::gfx::BaseProgram::Parameter{.matrix4x4 = model});
      bp->setParameter("skybox", rdm::gfx::DtSampler,
                       rdm::gfx::BaseProgram::Parameter{
                           .texture.slot = 0, .texture.texture = m_skybox});
      rdm::gfx::Model* _model =
          getGfxEngine()->getMeshCache()->get("dat0/cube.obj").value();
      _model->render(getGfxEngine()->getDevice());
    }

    cam.setPosition(
        (glm::mat3(r1) * glm::mat3(r2) * glm::vec3(0, 0, isGhost ? 100 : 5)) +
        cam.getTarget());
  });

  getWorld()->stepped.listen([this, dm] {
    if (PlayerInstance* player =
            dm->getRoot()->getService<PlayersService>()->getLocalPlayer()) {
      if (SoulInstance* soul = player->getCharacter()) {
        ModelInstance* model = dynamic_cast<ModelInstance*>(soul->getParent());
        if (model) {
          BlockInstance* head =
              dynamic_cast<BlockInstance*>(model->findFirstChildOfName("Body"));
          btRigidBody* body = head->getRigidBody();
          body->setAngularFactor(btVector3(0, 0, 1));
          body->setLinearFactor(btVector3(0, 0, 0));
        }
      }
    }
  });

  // dm->getRoot()->getService<PlayersService>()->createLocalPlayer();
}
}  // namespace freeblock
