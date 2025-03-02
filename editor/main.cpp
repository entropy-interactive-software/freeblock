#include <game.hpp>
#include <settings.hpp>

#include "datamodel.hpp"
#include "fb_game.hpp"
#include "instance.hpp"
#include "runservice.hpp"

static freeblock::Instance* selectedInstance = NULL;

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
  game.setEditor(true);
  game.earlyInit();
  game.getGfxEngine()->renderStepped.listen([&game] {
    ImGui::Begin("Editor");

    freeblock::DataModel* dm =
        (freeblock::DataModel*)game.getWorld()->getUser();
    freeblock::RunService* run =
        dm->getRoot()->getService<freeblock::RunService>();

    if (run->getState() != freeblock::RunService::Running) {
      if (ImGui::Button("Play")) {
        run->start();
      }
    } else {
      if (ImGui::Button("Stop")) {
        run->stop();
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
      }
      if (ImGui::Button("Delete")) {
        delete selectedInstance;
        selectedInstance = NULL;
      }

      ImGui::End();
    }
  });
  game.mainLoop();

  rdm::Settings::singleton()->save();
}
