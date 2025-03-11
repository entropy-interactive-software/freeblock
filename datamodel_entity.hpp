#pragma once
#include "datamodel.hpp"
#include "network/entity.hpp"
#include "pipeline.hpp"
namespace freeblock {
namespace net = rdm::network;
class DataModelTrackingEntity : public net::Entity {
  std::unique_ptr<DataModel> dm;
  std::unique_ptr<Pipeline> pipeline;
  glm::mat4 r1;
  glm::mat4 r2;
  rdm::gfx::BaseTexture* m_skybox;

 public:
  DataModelTrackingEntity(net::NetworkManager* manager, net::EntityId id);

  virtual const char* getTypeName() { return "datamodel"; };

  virtual void serialize(net::BitStream& stream);
  virtual void deserialize(net::BitStream& stream);

  virtual void tick();

  DataModel* getDM() { return dm.get(); }
};
}  // namespace freeblock
