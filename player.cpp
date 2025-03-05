#include "player.hpp"

#include "block.hpp"
#include "instance.hpp"
#include "joint.hpp"
#include "mesh.hpp"
#include "model.hpp"
#include "reflection.hpp"
#include "reflection_props.hpp"
#include "soul.hpp"
#include "workspace.hpp"
namespace freeblock {
INSTANCE_CTOR(PlayerInstance, Instance) {}

REFLECTION_BEGIN_DESCRIBED(PlayerInstance);
REFLECTION_PROPERTY_INSTANCE(PlayerInstance, Soul,
                             &PlayerInstance::getCharacter, NULL);
REFLECTION_END_DESCRIBED();

void PlayerInstance::spawnCharacter() {
  ModelInstance* playerModel =
      getService<WorkspaceInstance>()->createChild<ModelInstance>();
  SoulInstance* soul = playerModel->createChild<SoulInstance>();
  BlockInstance* block;
  playerModel->setName(getName());
  character = INSTANCE_TOUUID(soul);

  block = playerModel->createChild<BlockInstance>();
  block->setName("Body");
  block->setPosition(glm::vec3(0, 0, 0));
  block->setSize(glm::vec3(1.723, 1.413, 0.433));
  block->setAnchored(false);

  MeshInstance* mesh = block->createChild<MeshInstance>();
  mesh->setMeshPath("content/meshes/playermodel.obj");

  playerModel->setPrimaryBlock(block);
}
}  // namespace freeblock
