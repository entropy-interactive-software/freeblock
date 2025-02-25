#pragma once

#include "instance.hpp"
#include "soul.hpp"

namespace freeblock {
class ChocolateBlock : public Instance {
  INSTANCE(ChocolateBlock, Instance);

  bool eaten;

  enum ChocolateType { MILKY, NORMAL, DARK, POO, ALMOND, WHITE };

  ChocolateType type;

 public:
  ChocolateBlock();

  ChocolateType getChocolateType() { return type; };
  bool getEaten() { return eaten; };
  void eatChocolate(SoulInstance& soul);
};

}  // namespace freeblock
