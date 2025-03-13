#pragma once
#include "block.hpp"
#include "datamodel.hpp"
#include "instance.hpp"
namespace freeblock {
class ModelInstance : public Instance {
  INSTANCE(ModelInstance, Instance);
  DESCRIBED;

  bool dirty;
  InstanceUUID primaryBlock;

  Instance* getPrimaryBlock_R() {
    return getDM()->getInstanceByUUID<BlockInstance>(primaryBlock);
  }

  void setPrimaryBlock_R(Instance* block) {
    if (block && !block->isA("BlockInstance"))
      throw std::runtime_error("Block is not BlockInstance");
    primaryBlock = INSTANCE_TOUUID(block);
  }

 public:
  void setDirty(bool d) { dirty = d; };
  bool getDirty() { return dirty; }

  BlockInstance* getPrimaryBlock() {
    return getDM()->getInstanceByUUID<BlockInstance>(primaryBlock);
  }

  void setPrimaryBlock(BlockInstance* block) {
    primaryBlock = INSTANCE_TOUUID(block);
  }

  void moveTo(glm::vec3 position);
};
};  // namespace freeblock
