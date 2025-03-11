#pragma once
#include <game.hpp>

#include "datamodel.hpp"
#include "pipeline.hpp"
namespace freeblock {
class Game : public rdm::Game {
  std::unique_ptr<Pipeline> renderPipeline;

  bool editor;

 public:
  void setEditor(bool b) { editor = b; }
  void addEntityConstructors(rdm::network::NetworkManager* manager);

  virtual void initialize();
  virtual void initializeClient();
  virtual void initializeServer();
};
};  // namespace freeblock
