#include <game.hpp>
#include <settings.hpp>

#include "datamodel.hpp"
#include "datamodel_entity.hpp"
#include "fb_game.hpp"
#include "instance.hpp"
#include "players.hpp"
#include "reflection.hpp"
#include "reflection_props.hpp"
#include "runservice.hpp"

static freeblock::Instance* selectedInstance = NULL;
static bool createChildPrompt = NULL;

static void instance_tree(freeblock::Instance* instance) {
  ImGui::PushID(instance->getUUID().c_str());
  if (ImGui::TreeNode(
          (instance->getClassName() + " " + instance->getName()).c_str())) {
    if (selectedInstance != instance)
      if (ImGui::Button("Select")) {
        selectedInstance = instance;
      }

    ImGui::Text("Class: %s", instance->getClassName().c_str());
    ImGui::Text("UUID: %s", instance->getUUID().c_str());

    auto children = instance->getChildren();
    for (auto child : children) {
      instance_tree(child);
    }

    ImGui::TreePop();
  }
  ImGui::PopID();
}

int main(int argc, char** argv) {
  rdm::Settings::singleton()->parseCommandLine(argv, argc);
  freeblock::Game game;
  game.getWorldConstructorSettings().name = "freeblock";
  game.setEditor(true);
  game.earlyInit();
  game.getGfxEngine()->renderStepped.listen([&game] {
    freeblock::DataModelTrackingEntity* dm_e =
        (freeblock::DataModelTrackingEntity*)game.getWorld()
            ->getNetworkManager()
            ->getEntityById(0);
    if (!dm_e) return;
    freeblock::DataModel* dm = dm_e->getDM();

    freeblock::PlayersService* players =
        dm->getRoot()->getService<freeblock::PlayersService>();

    ImGui::Begin("Editor");

    freeblock::RunService* run =
        dm->getRoot()->getService<freeblock::RunService>();

    if (!players->getLocalPlayer()) {
      if (run->getState() != freeblock::RunService::Running) {
        if (ImGui::Button("Start Simulation")) {
          run->start();
        }
        if (ImGui::Button("Play")) {
          players->createLocalPlayer();
          run->start();
        }
        if (ImGui::Button("Load map.rbxl")) {
          dm->loadLegacyMap("map.rbxl");
        }
      } else {
        if (ImGui::Button("Stop")) {
          run->stop();
        }
      }
    }

    ImGui::End();

    ImGui::Begin("Tree");
    instance_tree(dm->getRoot());
    ImGui::End();

    if (selectedInstance) {
      ImGui::Begin("Instance");
      ImGui::Text("Name: %s", selectedInstance->getName().c_str());
      ImGui::Text("Class: %s", selectedInstance->getClassName().c_str());
      if (ImGui::Button("Unselect")) {
        selectedInstance = NULL;
        ImGui::End();
        return;
      }
      if (ImGui::Button("Delete")) {
        delete selectedInstance;
        selectedInstance = NULL;
        ImGui::End();
        return;
      }
      if (ImGui::Button("Create child")) {
        createChildPrompt = true;
      }

      freeblock::reflection::PropertyList list =
          selectedInstance->getProperties();
      for (auto property : list) {
        switch (property.second->getType()) {
          case freeblock::reflection::Property::String:
            ImGui::Text("%s \"%s\"", property.second->getName(),
                        property.second->getString(selectedInstance).c_str());
            break;
          case freeblock::reflection::Property::Vec3: {
            glm::vec3 v = property.second->getVec3(selectedInstance);
            ImGui::Text("%s (%0.2f, %0.2f, %0.2f)", property.second->getName(),
                        v.x, v.y, v.z);
          } break;
          case freeblock::reflection::Property::Bool:
            ImGui::Text(
                "%s %s", property.second->getName(),
                property.second->getBool(selectedInstance) ? "true" : "false");
            break;
          case freeblock::reflection::Property::InstanceRef: {
            freeblock::Instance* instance =
                property.second->getInstance(selectedInstance);
            ImGui::Text("%s %s", property.second->getName(),
                        instance ? instance->getName().c_str() : "nil");
          } break;
          case freeblock::reflection::Property::Function:
            break;
          default:
            ImGui::Text("%s, bad type", property.second->getName());
            break;
        }
      }

      ImGui::End();
    }

    if (createChildPrompt) {
      ImGui::Begin("Create");
      auto instances = freeblock::InstanceFactory::singleton()->getInstances();
      for (auto instance : instances) {
        if (ImGui::Button(instance.c_str())) {
          freeblock::Instance* i =
              freeblock::InstanceFactory::singleton()->create(instance.c_str(),
                                                              dm);
          i->setParent(selectedInstance);
          selectedInstance = i;
          createChildPrompt = false;
        }
      }

      if (ImGui::Button("Cancel")) {
        createChildPrompt = false;
      }
      ImGui::End();
    }
  });
  game.mainLoop();

  rdm::Settings::singleton()->save();
}
