#include "datamodel_entity.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>

#include "datamodel.hpp"
#include "datamodel_described.hpp"
#include "input.hpp"
#include "instance.hpp"
#include "model.hpp"
#include "pipeline.hpp"
#include "player.hpp"
#include "players.hpp"
#include "reflection_props.hpp"
#include "runservice.hpp"
#include "script_context.hpp"
#include "settings.hpp"
#include "workspace.hpp"

namespace freeblock {
void __t(net::NetworkManager* manager, net::CustomEventID id,
         net::BitStream stream) {}

DataModelTrackingEntity::DataModelTrackingEntity(net::NetworkManager* manager,
                                                 net::EntityId id)
    : net::Entity(manager, id) {
  dm.reset(new DataModel(getWorld()));
  DataModel* dmref = dm.get();
  r1 = glm::mat4(1);
  r2 = glm::mat4(1);
  m_skybox = NULL;

  if (!getManager()->isBackend()) {
    auto& signal = manager->getSignal(0x5e41);
    retransEvent = signal.listen(
        [this](rdm::network::BitStream& stream) { retransData(stream); });

    pipeline.reset(new Pipeline(getGfxEngine(), dm.get()));
    getGfxEngine()->renderStepped.listen([this, dmref] {
      if (!m_skybox) {
        rdm::gfx::Engine* engine = getGfxEngine();
        std::vector<void*> cubemap_textures = {
            engine->getTextureCache()
                ->getOrLoad2d(
                    "dat5/baseq3/textures/skies/null_plainsky512_rt.jpg", true)
                .value()
                .first.data,
            engine->getTextureCache()
                ->getOrLoad2d(
                    "dat5/baseq3/textures/skies/null_plainsky512_lf.jpg", true)
                .value()
                .first.data,
            engine->getTextureCache()
                ->getOrLoad2d(
                    "dat5/baseq3/textures/skies/null_plainsky512_dn.jpg", true)
                .value()
                .first.data,
            engine->getTextureCache()
                ->getOrLoad2d(
                    "dat5/baseq3/textures/skies/null_plainsky512_up.jpg", true)
                .value()
                .first.data,
            engine->getTextureCache()
                ->getOrLoad2d(
                    "dat5/baseq3/textures/skies/null_plainsky512_bk.jpg", true)
                .value()
                .first.data,
            engine->getTextureCache()
                ->getOrLoad2d(
                    "dat5/baseq3/textures/skies/null_plainsky512_ft.jpg", true)
                .value()
                .first.data,
        };
        std::unique_ptr<rdm::gfx::BaseTexture> skybox =
            engine->getDevice()->createTexture();
        skybox->uploadCubeMap(
            engine->getTextureCache()
                ->getOrLoad2d(
                    "dat5/baseq3/textures/skies/null_plainsky512_lf.jpg")
                .value()
                .first.width,
            engine->getTextureCache()
                ->getOrLoad2d(
                    "dat5/baseq3/textures/skies/null_plainsky512_lf.jpg")
                .value()
                .first.height,
            cubemap_textures);
        rdm::gfx::TextureCache::Info info;
        info.channels = 3;
        info.data = NULL;
        info.width = 256;
        info.height = 256;
        m_skybox = skybox.get();
        engine->getTextureCache()->cacheExistingTexture("null_plainsky512",
                                                        skybox, info);
      }

      rdm::gfx::Camera& cam = getGfxEngine()->getCamera();
      cam.setUp(glm::vec3(0, 1, 0));

      bool isGhost = false;
      if (PlayerInstance* player =
              dm->getRoot()->getService<PlayersService>()->getLocalPlayer()) {
        if (SoulInstance* soul = player->getCharacter()) {
          ModelInstance* model =
              dynamic_cast<ModelInstance*>(soul->getParent());
          if (model) {
            BlockInstance* head = dynamic_cast<BlockInstance*>(
                model->findFirstChildOfName("Body"));
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
        auto mt =
            getGfxEngine()->getMaterialCache()->getOrLoad("Skybox").value();
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
        bp->bind();
        rdm::gfx::Model* _model =
            getGfxEngine()->getMeshCache()->get("dat0/cube.obj").value();
        _model->render(getGfxEngine()->getDevice());
      }
      WorkspaceInstance* workspace =
          dm->getRoot()->getService<WorkspaceInstance>();
      if (workspace->getInfinitePlane()) {
        getGfxEngine()->getDevice()->setDepthState(
            rdm::gfx::BaseDevice::LEqual);
        auto mt = getGfxEngine()->getMaterialCache()->getOrLoad("Mesh").value();
        rdm::gfx::BaseProgram* bp =
            mt->prepareDevice(getGfxEngine()->getDevice(), 0);
        glm::mat4 model(1);
        glm::vec3 camPos = cam.getPosition();
        model = glm::translate(model, glm::vec3(camPos.x, 1, camPos.y));
        bp->setParameter("model", rdm::gfx::DtMat4,
                         rdm::gfx::BaseProgram::Parameter{.matrix4x4 = model});
        bp->bind();
        rdm::gfx::Model* _model = getGfxEngine()
                                      ->getMeshCache()
                                      ->get("content/meshes/infinite_plane.obj")
                                      .value();
        _model->render(getGfxEngine()->getDevice());
      }

      cam.setPosition(
          (glm::mat3(r1) * glm::mat3(r2) * glm::vec3(0, 0, isGhost ? 50 : 5)) +
          cam.getTarget());
    });

    getWorld()->stepped.listen([this, dmref] {
      if (PlayerInstance* player =
              dm->getRoot()->getService<PlayersService>()->getLocalPlayer()) {
        if (SoulInstance* soul = player->getCharacter()) {
          ModelInstance* model =
              dynamic_cast<ModelInstance*>(soul->getParent());
          if (model) {
            BlockInstance* head = dynamic_cast<BlockInstance*>(
                model->findFirstChildOfName("Body"));
            btRigidBody* body = head->getRigidBody();
            body->setAngularFactor(btVector3(0, 0, 1));
            body->setLinearFactor(btVector3(0, 0, 0));
          }
        }
      }
    });
  } else {
    // EVERY SERVICE must be created or the client will create its own services
    for (auto service : InstanceFactory::singleton()->getServices()) {
      InstanceFactory::singleton()->getService(service.c_str(), dm.get());
    }

    dm->getRoot()->getService<ScriptContext>()->executeScriptFile(
        "content/scripts/start.lua");
    // dm->loadLegacyMap("map.rbxl");
  }
}

DataModelTrackingEntity::~DataModelTrackingEntity() {
  if (!getManager()->isBackend()) {
    auto& signal = getManager()->getSignal(0x5e41);
    signal.removeListener(retransEvent);
  }
}

enum EntryType {
  ROOTINSTANCE,
  INSTANCE,
  INSTANCE_PROP,
  STOP,
};

void DataModelTrackingEntity::tick() {
  RunService* run = dm->getRoot()->getService<RunService>();
  run->_step();

  if (getDM()->isServer() && getManager()->getPeers().size()) {
    auto dirty = dm->getDirtyInstances();
    if (dirty.size()) {
      net::BitStream stream;
      for (auto uuid : dirty) {
        Instance* instance = dm->getInstanceByUUID(uuid);
        writeInstanceProperties(instance, stream);
      }
      stream.write<EntryType>(STOP);
      getManager()->sendCustomEvent(0x5e41, stream);
    }
  }
}

void DataModelTrackingEntity::retransData(net::BitStream& stream) {
  std::scoped_lock l(getDM()->getMutex());
  readDataPacket(stream);
  pipeline->regenerateAll();
}

void DataModelTrackingEntity::writeInstanceProperties(Instance* instance,
                                                      net::BitStream& stream) {
  bool trackable = InstanceFactory::singleton()->isTrackable(
      instance->getClassName().c_str());
  if (!trackable) return;
  stream.write<EntryType>(INSTANCE_PROP);
  stream.writeString(instance->getUUID());
  auto plist = instance->getProperties();
  for (auto prop : plist) {
    if (!prop.second->isWriteable()) continue;
    stream.writeString(prop.second->getName());
    switch (prop.second->getType()) {
      case reflection::Property::String:
        stream.writeString(prop.second->getString(instance));
        break;
      case reflection::Property::InstanceRef:
        stream.writeString(INSTANCE_TOUUID(prop.second->getInstance(instance)));
        break;
      case reflection::Property::Vec3:
        stream.write<glm::vec3>(prop.second->getVec3(instance));
        break;
      case reflection::Property::Mat3:
        stream.write<glm::mat3>(prop.second->getMat3(instance));
        break;
      default:
        break;
    }
  }
  stream.writeString("");
}

void DataModelTrackingEntity::serialize(net::BitStream& stream) {
  std::scoped_lock l(getDM()->getMutex());

  stream.write<EntryType>(ROOTINSTANCE);
  stream.writeString(getDM()->getRoot()->getUUID());

  // INSTANCES
  for (auto [uuid, instance] : dm->instances) {
    bool trackable = InstanceFactory::singleton()->isTrackable(
        instance->getClassName().c_str());
    if (!trackable) continue;

    stream.write<EntryType>(INSTANCE);
    stream.writeString(instance->getUUID());
    stream.writeString(instance->getClassName());
    stream.write<bool>(dm->getRoot() == instance);
  }

  // INSTANCE PROPERTIES
  for (auto [uuid, instance] : dm->instances) {
    writeInstanceProperties(instance, stream);
  }

  stream.write<EntryType>(STOP);
}

void DataModelTrackingEntity::readDataPacket(net::BitStream& stream) {
  DataModelDescribed* newDM = (DataModelDescribed*)getDM()->getRoot();
  bool setupNewDm = false;

  bool processing = true;
  while (processing) {
    switch (stream.read<EntryType>()) {
      case ROOTINSTANCE:
        newDM = new DataModelDescribed(getDM(), stream.readString());
        setupNewDm = true;
        break;
      case INSTANCE:
        try {
          std::string uuid = stream.readString();
          std::string type = stream.readString();

          if (!InstanceFactory::singleton()->isTrackable(type.c_str())) {
            throw std::runtime_error("Type not trackable");
          }

          bool root = stream.read<bool>();

          if (root) {
          } else {
            InstanceFactory::singleton()->createRemote(type.c_str(), uuid,
                                                       dm.get());
          }

        } catch (std::exception& e) {
          rdm::Log::printf(rdm::LOG_ERROR, "Processing error: %s", e.what());
          processing = false;
        }
        break;
      case INSTANCE_PROP:
        try {
          std::string uuid = stream.readString();
          Instance* instance = dm->getInstanceByUUID(uuid);
          if (!instance) {
            rdm::Log::printf(rdm::LOG_ERROR, "UUID %s is null", uuid.c_str());
            throw std::runtime_error("Invalid instance");
          }
          auto plist = instance->getProperties();
          while (true) {
            std::string pname = stream.readString();
            if (pname.empty()) break;
            auto prop = plist.find(pname);
            if (prop == plist.end()) {
              rdm::Log::printf(rdm::LOG_ERROR, "Read property name %s",
                               pname.c_str());
              throw std::runtime_error("Invalid property name");
            }
            if (!prop->second->isWriteable()) continue;
            switch (prop->second->getType()) {
              case reflection::Property::String:
                prop->second->setString(instance, stream.readString());
                break;
              case reflection::Property::InstanceRef: {
                std::string nuuid = stream.readString();
                if (nuuid == "nil") {
                  prop->second->setInstance(instance, NULL);
                  break;
                }
                if (Instance* v = dm->getInstanceByUUID(nuuid)) {
                  prop->second->setInstance(instance, v);
                } else {
                  rdm::Log::printf(
                      rdm::LOG_WARN,
                      "InstanceRef %s (%s, %s) '%s' = %s, which is "
                      "unknown to the client",
                      instance->getName().c_str(),
                      instance->getClassName().c_str(), uuid.c_str(),
                      prop->first.c_str(), nuuid.c_str());
                }
              } break;
              case reflection::Property::Vec3: {
                prop->second->setVec3(instance, stream.read<glm::vec3>());
              } break;
              case reflection::Property::Mat3:
                prop->second->setMat3(instance, stream.read<glm::mat3>());
                break;
              default:
                break;
            }
          }
        } catch (std::exception& e) {
          rdm::Log::printf(rdm::LOG_ERROR, "Processing error: %s", e.what());
          processing = false;
        }
        break;
      default:
        rdm::Log::printf(rdm::LOG_ERROR,
                         "Received weird entry, stopping processing");
      case STOP:
        processing = false;
        break;
    }
  }

  if (setupNewDm) {
    getDM()->root = newDM;
  }
}

void DataModelTrackingEntity::deserialize(net::BitStream& stream) {
  std::scoped_lock l(getDM()->getMutex());

  readDataPacket(stream);

  pipeline->regenerateAll();
}
};  // namespace freeblock
