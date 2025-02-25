#pragma once
#include "instance.hpp"
namespace freeblock {
class SoulInstance : public Instance {
  INSTANCE(SoulInstance, Instance);

  unsigned int health;
  enum State {
    DEAD,
    WALKING,
    IDLE,
    IN_AIR,
  };

  State state;
  std::string name;

 public:
  State getState() { return state; };
  std::string getName() { return name; };

  void setName(std::string name) { this->name = name; };
  void setHealth(unsigned int health) { this->health = health; };
  void setState(State state) { this->state = state; };

  void kill();
  void respawn();
};
}  // namespace freeblock
