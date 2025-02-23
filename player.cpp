#include "player.hpp"

#include "block.hpp"
#include "instance.hpp"
#include "model.hpp"
#include "soul.hpp"
#include "workspace.hpp"
namespace freeblock {
INSTANCE_CTOR(PlayerInstance, Instance) {}

void PlayerInstance::spawnCharacter() {
  ModelInstance* playerModel =
      getService<WorkspaceInstance>()->createChild<ModelInstance>();
  SoulInstance* soul = playerModel->createChild<SoulInstance>();
  BlockInstance* block;
  playerModel->setName(getName());
  character = INSTANCE_TOUUID(soul);
  for (int i = 0; i < 6; i++) {
    switch (i) {
      case 0:  // head
        block = playerModel->createChild<BlockInstance>();
        block->setName("Head");
        block->setPosition(glm::vec3(0, 1.5, 0));
        block->setSize(glm::vec3(2, 1, 1));
        break;
      case 1:  // torso
        block = playerModel->createChild<BlockInstance>();
        block->setName("Torso");
        block->setPosition(glm::vec3(0, 0.0, 0));
        block->setSize(glm::vec3(2, 2, 1));
        playerModel->setPrimaryBlock(block);
        break;
      case 2:  // left arm
        block = playerModel->createChild<BlockInstance>();
        block->setName("Left arm");
        block->setPosition(glm::vec3(1.5, 0, 0));
        block->setSize(glm::vec3(1, 2, 1));
        break;
      case 3:  // right arm
        block = playerModel->createChild<BlockInstance>();
        block->setName("Right arm");
        block->setPosition(glm::vec3(-1.5, 0, 0));
        block->setSize(glm::vec3(1, 2, 1));
        break;
      case 4:  // left arm
        block = playerModel->createChild<BlockInstance>();
        block->setName("Left arm");
        block->setPosition(glm::vec3(0.5, -2.0, 0));
        block->setSize(glm::vec3(1, 2, 1));
        break;
      case 5:  // right arm
        block = playerModel->createChild<BlockInstance>();
        block->setName("Right arm");
        block->setPosition(glm::vec3(-0.5, -2.0, 0));
        block->setSize(glm::vec3(1, 2, 1));
        break;
    }
  }
  playerModel->moveTo(glm::vec3(10, 10, 30));
}
}  // namespace freeblock
