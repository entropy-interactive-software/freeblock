#pragma once
#include <gfx/engine.hpp>

#include "datamodel.hpp"
namespace freeblock {
class Pipeline {
  DataModel* dataModel;
  rdm::gfx::Engine* engine;

  rdm::ClosureId renderJob;

 public:
  Pipeline(rdm::gfx::Engine* engine, DataModel* dm);

  void render();

  ~Pipeline();
};
};  // namespace freeblock
