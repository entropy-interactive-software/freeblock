#pragma once
#include "service.hpp"
namespace freeblock {
class RunService : public Service {
  DESCRIBED;
  INSTANCE(RunService, Service);

  friend class DataModelTrackingEntity;

  void updateSimulation();

  void _step();

  btCollisionShape* infinitePlaneShape;
  btRigidBody* infinitePlaneObject;

  static int luaStart(lua_State* L);

 public:
  virtual ~RunService();

  enum State {
    Stopped,
    Paused,
    Running,
  };

  void start();
  void stop();

  State getState() { return state; };

  rdm::Signal<> stepped;

 private:
  State state;
};
};  // namespace freeblock
