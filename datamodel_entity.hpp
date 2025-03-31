#pragma once
#include "datamodel.hpp"
#include "instance.hpp"
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
  void readDataPacket(net::BitStream& stream);
  rdm::ClosureId retransEvent;

  void retransData(net::BitStream& stream);
  void writeInstanceProperties(Instance* instance, net::BitStream& stream);

 public:
  DataModelTrackingEntity(net::NetworkManager* manager, net::EntityId id);
  virtual ~DataModelTrackingEntity();

  virtual const char* getTypeName() { return "datamodel"; };

  virtual void serialize(net::BitStream& stream);
  virtual void deserialize(net::BitStream& stream);

  virtual void tick();

  DataModel* getDM() { return dm.get(); }
};
}  // namespace freeblock
