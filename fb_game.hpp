#pragma once
#include <game.hpp>

#include "datamodel.hpp"
#include "pipeline.hpp"
namespace freeblock {
class Game : public rdm::Game {
  std::unique_ptr<Pipeline> renderPipeline;

  DataModel* createDM(rdm::World* world);

  glm::mat4 r1;
  glm::mat4 r2;
  bool editor;

  rdm::gfx::BaseTexture* m_skybox;

 public:
  void setEditor(bool b) { editor = b; }

  virtual void initialize();
  virtual void initializeClient();
  virtual void initializeServer();
};
};  // namespace freeblock
