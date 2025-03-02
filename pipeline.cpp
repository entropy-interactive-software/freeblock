#include "pipeline.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "block.hpp"
#include "mesh.hpp"
#include "model.hpp"
#include "palette.hpp"
#include "soul.hpp"
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
    int surface;

    Vertex(glm::vec3 p, glm::vec3 c, glm::vec3 n, int s) {
      position = p;
      color = c;
      normal = n;
      surface = s;
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
    cluster.arrayPointers->addAttrib(rdm::gfx::BaseArrayPointers::Attrib(
        rdm::gfx::DtInt, 3, 1, sizeof(Vertex), (void*)offsetof(Vertex, surface),
        cluster.vertexBuffer.get()));
    cluster.modelUuid = INSTANCE_TOUUID(model);
    cluster.trackedBlocks = 0;

    cluster.dirty = true;
  }

  Cluster& cluster = clusters[model->getUUID()];

  if (model->getDirty()) {
    cluster.dirty = true;
    model->setDirty(false);
  }

  std::vector<BlockInstance*> blocks;
  for (auto child : model->getChildren()) {
    if (ModelInstance* _model = dynamic_cast<ModelInstance*>(child)) {
      this->model(_model);
    } else if (BlockInstance* block = dynamic_cast<BlockInstance*>(child)) {
      if (block->findFirstChildOfType<MeshInstance>()) {
        meshesToRender.push_back(INSTANCE_TOUUID(block));
        continue;
      }
      if (cluster.dirty) {
        blocks.push_back(block);
      }
    }
  }

  // GEOMETRY GENERATION
  if (cluster.dirty) {
    {
      SoulInstance* soul = model->findFirstChildOfType<SoulInstance>();
      cluster.isSoul = soul ? true : false;
    }

    PVInstance* primaryBlock = model->getPrimaryBlock();
    if (!primaryBlock && blocks.size()) {
      primaryBlock = model->findFirstChildOfType<PVInstance>();
    }

    glm::vec3 basePosition = glm::vec3(0.0);
    glm::mat3 baseBasis = glm::mat3(1.0);
    if (primaryBlock) {
      basePosition = primaryBlock->getPosition();
      baseBasis = primaryBlock->getBasis();
      cluster.primaryPV = primaryBlock->getUUID();
    }

    bool pushElements = false;
    if (cluster.trackedBlocks != blocks.size()) pushElements = true;
    cluster.trackedBlocks = blocks.size();

    std::vector<Vertex> vertices;
    std::vector<unsigned int> elements;
    for (auto block : blocks) {
      glm::vec3 brickColor = Palette::blockColorToColor(block->getColor());
      glm::vec3 brickSize = block->getSize();
      glm::vec3 brickPosition = block->getPosition() - basePosition;
      glm::mat3 brickBasis = glm::inverse(block->getBasis());
      switch (block->getShape()) {
        default:
        case BlockInstance::Cuboid:
          // right face
          {
            int index = vertices.size();

            vertices.push_back(
                Vertex(((brickSize * glm::vec3(0.5, -0.5, -0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(1, 0, 0) * brickBasis, 0));
            vertices.push_back(
                Vertex(((brickSize * glm::vec3(0.5, -0.5, 0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(1, 0, 0) * brickBasis, 0));
            vertices.push_back(
                Vertex(((brickSize * glm::vec3(0.5, 0.5, -0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(1, 0, 0) * brickBasis, 0));
            vertices.push_back(
                Vertex(((brickSize * glm::vec3(0.5, 0.5, 0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(1, 0, 0) * brickBasis, 0));

            if (pushElements) {
              elements.push_back(index + 0);
              elements.push_back(index + 1);
              elements.push_back(index + 2);

              elements.push_back(index + 2);
              elements.push_back(index + 1);
              elements.push_back(index + 3);
            }
          }
          // left face
          {
            int index = vertices.size();

            vertices.push_back(Vertex(
                ((brickSize * glm::vec3(-0.5, -0.5, -0.5)) * brickBasis) +
                    brickPosition,
                brickColor, glm::vec3(-1, 0, 0) * brickBasis, 1));
            vertices.push_back(
                Vertex(((brickSize * glm::vec3(-0.5, 0.5, -0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(-1, 0, 0) * brickBasis, 1));
            vertices.push_back(
                Vertex(((brickSize * glm::vec3(-0.5, -0.5, 0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(-1, 0, 0) * brickBasis, 1));
            vertices.push_back(
                Vertex(((brickSize * glm::vec3(-0.5, 0.5, 0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(-1, 0, 0) * brickBasis, 1));

            if (pushElements) {
              elements.push_back(index + 0);
              elements.push_back(index + 1);
              elements.push_back(index + 2);

              elements.push_back(index + 2);
              elements.push_back(index + 1);
              elements.push_back(index + 3);
            }
          }
          // top face
          {
            int index = vertices.size();

            vertices.push_back(
                Vertex(((brickSize * glm::vec3(-0.5, 0.5, -0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(0, 1, 0) * brickBasis, 2));
            vertices.push_back(
                Vertex(((brickSize * glm::vec3(0.5, 0.5, -0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(0, 1, 0) * brickBasis, 2));
            vertices.push_back(
                Vertex(((brickSize * glm::vec3(-0.5, 0.5, 0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(0, 1, 0) * brickBasis, 2));
            vertices.push_back(
                Vertex(((brickSize * glm::vec3(0.5, 0.5, 0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(0, 1, 0) * brickBasis, 2));

            if (pushElements) {
              elements.push_back(index + 0);
              elements.push_back(index + 1);
              elements.push_back(index + 2);

              elements.push_back(index + 2);
              elements.push_back(index + 1);
              elements.push_back(index + 3);
            }
          }
          // bottom face
          {
            int index = vertices.size();

            vertices.push_back(Vertex(
                ((brickSize * glm::vec3(-0.5, -0.5, -0.5)) * brickBasis) +
                    brickPosition,
                brickColor, glm::vec3(0, -1, 0) * brickBasis, 3));
            vertices.push_back(
                Vertex(((brickSize * glm::vec3(-0.5, -0.5, 0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(0, -1, 0) * brickBasis, 3));
            vertices.push_back(
                Vertex(((brickSize * glm::vec3(0.5, -0.5, -0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(0, -1, 0) * brickBasis, 3));
            vertices.push_back(
                Vertex(((brickSize * glm::vec3(0.5, -0.5, 0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(0, -1, 0) * brickBasis, 3));

            if (pushElements) {
              elements.push_back(index + 0);
              elements.push_back(index + 1);
              elements.push_back(index + 2);

              elements.push_back(index + 2);
              elements.push_back(index + 1);
              elements.push_back(index + 3);
            }
          }
          // front face
          {
            int index = vertices.size();

            vertices.push_back(
                Vertex(((brickSize * glm::vec3(-0.5, -0.5, 0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(0, 0, 1) * brickBasis, 4));
            vertices.push_back(
                Vertex(((brickSize * glm::vec3(0.5, -0.5, 0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(0, 0, 1) * brickBasis, 4));
            vertices.push_back(
                Vertex(((brickSize * glm::vec3(-0.5, 0.5, 0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(0, 0, 1) * brickBasis, 4));
            vertices.push_back(
                Vertex(((brickSize * glm::vec3(0.5, 0.5, 0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(0, 0, 1) * brickBasis, 4));

            if (pushElements) {
              elements.push_back(index + 0);
              elements.push_back(index + 1);
              elements.push_back(index + 2);

              elements.push_back(index + 2);
              elements.push_back(index + 1);
              elements.push_back(index + 3);
            }
          }
          // back face
          {
            int index = vertices.size();

            vertices.push_back(Vertex(
                ((brickSize * glm::vec3(-0.5, -0.5, -0.5)) * brickBasis) +
                    brickPosition,
                brickColor, glm::vec3(0, 0, -1), 5));
            vertices.push_back(
                Vertex(((brickSize * glm::vec3(0.5, -0.5, -0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(0, 0, -1), 5));
            vertices.push_back(
                Vertex(((brickSize * glm::vec3(-0.5, 0.5, -0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(0, 0, -1), 5));
            vertices.push_back(
                Vertex(((brickSize * glm::vec3(0.5, 0.5, -0.5)) * brickBasis) +
                           brickPosition,
                       brickColor, glm::vec3(0, 0, -1), 5));

            if (pushElements) {
              elements.push_back(index + 0);
              elements.push_back(index + 1);
              elements.push_back(index + 2);

              elements.push_back(index + 2);
              elements.push_back(index + 1);
              elements.push_back(index + 3);
            }
          }
          break;
      }
    }

    cluster.vertexBuffer->upload(
        rdm::gfx::BaseBuffer::Array, rdm::gfx::BaseBuffer::DynamicDraw,
        sizeof(Vertex) * vertices.size(), vertices.data());
    if (pushElements) {
      cluster.elementBuffer->upload(
          rdm::gfx::BaseBuffer::Element, rdm::gfx::BaseBuffer::StaticDraw,
          sizeof(unsigned int) * elements.size(), elements.data());
      cluster.count = elements.size();

      rdm::Log::printf(rdm::LOG_DEBUG,
                       "Generated mesh for UUID %s (%i elem, %ib)",
                       model->getUUID().c_str(), cluster.count,
                       sizeof(Vertex) * vertices.size());
    }
    cluster.arrayPointers->upload();
    cluster.dirty = false;
  }
}

void Pipeline::render() {
  WorkspaceInstance* workspace =
      dataModel->getRoot()->getService<WorkspaceInstance>();
  model(workspace);

  auto mt = engine->getMaterialCache()->getOrLoad("Cluster").value();
  rdm::gfx::BaseProgram* bp = mt->prepareDevice(engine->getDevice(), 0);
  rdm::gfx::BaseProgram::Parameter param;
  param.texture.slot = 0;
  param.texture.texture = engine->getTextureCache()
                              ->getOrLoad2d("content/textures/outlet.png")
                              .value()
                              .second;
  bp->setParameter("outlet", rdm::gfx::DtSampler, param);
  rdm::gfx::RenderListSettings settings;
  settings.cull = rdm::gfx::BaseDevice::None;
  settings.state = rdm::gfx::BaseDevice::LEqual;
  rdm::gfx::RenderList list(bp, NULL, settings);
  for (auto& [uuid, cluster] : clusters) {
    if (cluster.count == 0) continue;
    if (!dataModel->getInstanceByUUID<ModelInstance>(cluster.modelUuid)) {
      continue;
    }

    rdm::gfx::RenderCommand command(rdm::gfx::BaseDevice::Triangles,
                                    cluster.elementBuffer.get(), cluster.count,
                                    cluster.arrayPointers.get());
    glm::mat4 model = glm::mat4(1);
    if (PVInstance* pv =
            dataModel->getInstanceByUUID<PVInstance>(cluster.primaryPV)) {
      // model *= glm::mat4(pv->getBasis());
      model = glm::translate(model, pv->getPosition());
    }
    command.setModel(model);
    list.add(command);
  }
  engine->pass(rdm::gfx::RenderPass::Opaque).add(list);

  std::shared_ptr<rdm::gfx::Material> material =
      engine->getMaterialCache()->getOrLoad("Mesh").value();
  rdm::gfx::BaseProgram* program =
      material->prepareDevice(engine->getDevice(), 0);
  for (auto uuid : meshesToRender) {
    BlockInstance* block = dataModel->getInstanceByUUID<BlockInstance>(uuid);
    MeshInstance* mesh = block->findFirstChildOfType<MeshInstance>();
    if (mesh) {
      glm::mat4 model(1);
      model *= glm::mat4(block->getBasis());
      model = glm::translate(model, block->getPosition());
      program->setParameter(
          "model", rdm::gfx::DtMat4,
          rdm::gfx::BaseProgram::Parameter{.matrix4x4 = model});
      rdm::gfx::Model* _model =
          engine->getMeshCache()->get(mesh->getMeshPath().c_str()).value();
      _model->render(engine->getDevice());
    }
  }
  meshesToRender.clear();
}
}  // namespace freeblock
