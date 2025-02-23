#pragma once
#include <gfx/engine.hpp>
#include <memory>

#include "block.hpp"
#include "datamodel.hpp"
#include "model.hpp"
namespace freeblock {

class Pipeline {
  DataModel* dataModel;
  rdm::gfx::Engine* engine;

  rdm::ClosureId renderJob;

  struct Cluster {
    bool dirty;
    bool isSoul;
    std::unique_ptr<rdm::gfx::BaseBuffer> vertexBuffer;
    std::unique_ptr<rdm::gfx::BaseBuffer> elementBuffer;
    std::unique_ptr<rdm::gfx::BaseArrayPointers> arrayPointers;
    InstanceUUID modelUuid;
    size_t count;
  };

  std::unordered_map<InstanceUUID, Cluster> clusters;
  void model(ModelInstance* model);

 public:
  Pipeline(rdm::gfx::Engine* engine, DataModel* dm);

  void render();

  ~Pipeline();
};
};  // namespace freeblock
