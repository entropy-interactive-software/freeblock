#pragma once
#include <game.hpp>

#include "datamodel.hpp"
#include "pipeline.hpp"
namespace freeblock {
class Game : public rdm::Game {
  std::unique_ptr<Pipeline> renderPipeline;

 public:
  virtual void initialize();
  virtual void initializeClient();
};
};  // namespace freeblock
