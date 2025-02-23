#include "pipeline.hpp"

#include "block.hpp"
#include "model.hpp"
#include "palette.hpp"
#include "workspace.hpp"

namespace freeblock {
Pipeline::Pipeline(rdm::gfx::Engine* engine, DataModel* dm) {
  renderJob = engine->renderStepped.listen([this] { render(); });
  this->engine = engine;
  this->dataModel = dm;
}

Pipeline::~Pipeline() { engine->renderStepped.removeListener(renderJob); }

void Pipeline::model(ModelInstance* model) {
  struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 normal;

    Vertex(glm::vec3 p, glm::vec3 c, glm::vec3 n) {
      position = p;
      color = c;
      normal = n;
    }
  };

  auto it = clusters.find(model->getUUID());
  if (it == clusters.end()) {
    clusters[model->getUUID()] = Cluster();
    Cluster& cluster = clusters[model->getUUID()];
    cluster.elementBuffer = engine->getDevice()->createBuffer();
    cluster.vertexBuffer = engine->getDevice()->createBuffer();
    cluster.arrayPointers = engine->getDevice()->createArrayPointers();
    cluster.arrayPointers->addAttrib(rdm::gfx::BaseArrayPointers::Attrib(
        rdm::gfx::DtFloat, 0, 3, sizeof(Vertex),
        (void*)offsetof(Vertex, position), cluster.vertexBuffer.get()));
    cluster.arrayPointers->addAttrib(rdm::gfx::BaseArrayPointers::Attrib(
        rdm::gfx::DtFloat, 1, 3, sizeof(Vertex),
        (void*)offsetof(Vertex, normal), cluster.vertexBuffer.get()));
    cluster.arrayPointers->addAttrib(rdm::gfx::BaseArrayPointers::Attrib(
        rdm::gfx::DtFloat, 2, 3, sizeof(Vertex), (void*)offsetof(Vertex, color),
        cluster.vertexBuffer.get()));

    cluster.dirty = true;
  }

  Cluster& cluster = clusters[model->getUUID()];
  std::vector<BlockInstance*> blocks;
  for (auto child : model->getChildren()) {
    if (ModelInstance* _model = dynamic_cast<ModelInstance*>(child)) {
      this->model(_model);
    } else if (BlockInstance* block = dynamic_cast<BlockInstance*>(child)) {
      if (cluster.dirty) {
        blocks.push_back(block);
      }
    }
  }

  // GEOMETRY GENERATION
  if (cluster.dirty) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> elements;
    for (auto block : blocks) {
      glm::vec3 brickColor = Palette::blockColorToColor(block->getColor());
      glm::vec3 brickSize = block->getSize();
      glm::vec3 brickPosition = block->getPosition();
      glm::mat3 brickBasis = block->getBasis();
      switch (block->getShape()) {
        default:
        case BlockInstance::Cuboid:
          // right face
          {
            int index = vertices.size();

            vertices.push_back(
                Vertex(((brickSize * glm::vec3(0.5, -0.5, -0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(1, 0, 0)));
            vertices.push_back(
                Vertex(((brickSize * glm::vec3(0.5, -0.5, 0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(1, 0, 0)));
            vertices.push_back(
                Vertex(((brickSize * glm::vec3(0.5, 0.5, -0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(1, 0, 0)));
            vertices.push_back(
                Vertex(((brickSize * glm::vec3(0.5, 0.5, 0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(1, 0, 0)));

            elements.push_back(index + 0);
            elements.push_back(index + 1);
            elements.push_back(index + 2);

            elements.push_back(index + 2);
            elements.push_back(index + 1);
            elements.push_back(index + 3);
          }
          // left face
          {
            int index = vertices.size();

            vertices.push_back(Vertex(
                ((brickSize * glm::vec3(-0.5, -0.5, -0.5)) * brickBasis) +
                    brickPosition,
                brickColor, glm::vec3(-1, 0, 0)));
            vertices.push_back(
                Vertex(((brickSize * glm::vec3(-0.5, 0.5, -0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(-1, 0, 0)));
            vertices.push_back(
                Vertex(((brickSize * glm::vec3(-0.5, -0.5, 0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(-1, 0, 0)));
            vertices.push_back(
                Vertex(((brickSize * glm::vec3(-0.5, 0.5, 0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(-1, 0, 0)));

            elements.push_back(index + 0);
            elements.push_back(index + 1);
            elements.push_back(index + 2);

            elements.push_back(index + 2);
            elements.push_back(index + 1);
            elements.push_back(index + 3);
          }
          // top face
          {
            int index = vertices.size();

            vertices.push_back(
                Vertex(((brickSize * glm::vec3(-0.5, 0.5, -0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(0, 1, 0)));
            vertices.push_back(
                Vertex(((brickSize * glm::vec3(0.5, 0.5, -0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(0, 1, 0)));
            vertices.push_back(
                Vertex(((brickSize * glm::vec3(-0.5, 0.5, 0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(0, 1, 0)));
            vertices.push_back(
                Vertex(((brickSize * glm::vec3(0.5, 0.5, 0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(0, 1, 0)));

            elements.push_back(index + 0);
            elements.push_back(index + 1);
            elements.push_back(index + 2);

            elements.push_back(index + 2);
            elements.push_back(index + 1);
            elements.push_back(index + 3);
          }
          // bottom face
          {
            int index = vertices.size();

            vertices.push_back(Vertex(
                ((brickSize * glm::vec3(-0.5, -0.5, -0.5)) * brickBasis) +
                    brickPosition,
                brickColor, glm::vec3(0, -1, 0)));
            vertices.push_back(
                Vertex(((brickSize * glm::vec3(-0.5, -0.5, 0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(0, -1, 0)));
            vertices.push_back(
                Vertex(((brickSize * glm::vec3(0.5, -0.5, -0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(0, -1, 0)));
            vertices.push_back(
                Vertex(((brickSize * glm::vec3(0.5, -0.5, 0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(0, -1, 0)));

            elements.push_back(index + 0);
            elements.push_back(index + 1);
            elements.push_back(index + 2);

            elements.push_back(index + 2);
            elements.push_back(index + 1);
            elements.push_back(index + 3);
          }
          // front face
          {
            int index = vertices.size();

            vertices.push_back(
                Vertex(((brickSize * glm::vec3(-0.5, -0.5, 0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(0, 0, 1)));
            vertices.push_back(
                Vertex(((brickSize * glm::vec3(0.5, -0.5, 0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(0, 0, 1)));
            vertices.push_back(
                Vertex(((brickSize * glm::vec3(-0.5, 0.5, 0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(0, 0, 1)));
            vertices.push_back(
                Vertex(((brickSize * glm::vec3(0.5, 0.5, 0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(0, 0, 1)));

            elements.push_back(index + 0);
            elements.push_back(index + 1);
            elements.push_back(index + 2);

            elements.push_back(index + 2);
            elements.push_back(index + 1);
            elements.push_back(index + 3);
          }
          // back face
          {
            int index = vertices.size();

            vertices.push_back(Vertex(
                ((brickSize * glm::vec3(-0.5, -0.5, -0.5)) * brickBasis) +
                    brickPosition,
                brickColor, glm::vec3(0, 0, -1)));
            vertices.push_back(
                Vertex(((brickSize * glm::vec3(0.5, -0.5, -0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(0, 0, -1)));
            vertices.push_back(
                Vertex(((brickSize * glm::vec3(-0.5, 0.5, -0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(0, 0, -1)));
            vertices.push_back(
                Vertex(((brickSize * glm::vec3(0.5, 0.5, -0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(0, 0, -1)));

            elements.push_back(index + 0);
            elements.push_back(index + 1);
            elements.push_back(index + 2);

            elements.push_back(index + 2);
            elements.push_back(index + 1);
            elements.push_back(index + 3);
          }
          break;
      }
    }

    cluster.vertexBuffer->upload(
        rdm::gfx::BaseBuffer::Array, rdm::gfx::BaseBuffer::DynamicDraw,
        sizeof(Vertex) * vertices.size(), vertices.data());
    cluster.elementBuffer->upload(
        rdm::gfx::BaseBuffer::Element, rdm::gfx::BaseBuffer::StaticDraw,
        sizeof(unsigned int) * elements.size(), elements.data());
    cluster.arrayPointers->upload();
    cluster.count = elements.size();
    cluster.dirty = false;

    rdm::Log::printf(rdm::LOG_DEBUG,
                     "Generated mesh for UUID %s (%i elem, %ib)",
                     model->getUUID().c_str(), cluster.count,
                     sizeof(Vertex) * vertices.size());
  }
}

void Pipeline::render() {
  WorkspaceInstance* workspace =
      dataModel->getRoot()->getService<WorkspaceInstance>();
  model(workspace);

  auto mt = engine->getMaterialCache()->getOrLoad("Mesh").value();
  rdm::gfx::BaseProgram* bp = mt->prepareDevice(engine->getDevice(), 0);
  rdm::gfx::RenderList list(bp, NULL);
  for (auto& [uuid, cluster] : clusters) {
    rdm::gfx::RenderCommand command(rdm::gfx::BaseDevice::Triangles,
                                    cluster.elementBuffer.get(), cluster.count,
                                    cluster.arrayPointers.get());
    list.add(command);
  }
  engine->pass(rdm::gfx::RenderPass::Opaque).add(list);
}
}  // namespace freeblock
