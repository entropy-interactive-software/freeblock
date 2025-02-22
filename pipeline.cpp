#include "pipeline.hpp"

#include "workspace.hpp"

namespace freeblock {
Pipeline::Pipeline(rdm::gfx::Engine* engine, DataModel* dm) {
  renderJob = engine->renderStepped.listen([this] { render(); });
  this->engine = engine;
  this->dataModel = dm;
}

Pipeline::~Pipeline() { engine->renderStepped.removeListener(renderJob); }

void Pipeline::render() {
  WorkspaceInstance* workspace =
      dataModel->getRoot()->getService<WorkspaceInstance>();
}
}  // namespace freeblock
