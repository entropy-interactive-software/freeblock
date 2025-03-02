#pragma once
#include "service.hpp"
namespace freeblock {
class RunService : public Service {
  INSTANCE(RunService, Service);

  void updateSimulation();
  void step();

 public:
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
