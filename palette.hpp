#pragma once
#include "glm/glm.hpp"

namespace freeblock {
typedef unsigned int BlockColor;

class Palette {
 public:
  static glm::vec3 blockColorToColor(BlockColor color);
  static BlockColor colorToBlockColor(glm::vec3 color);
};
};  // namespace freeblock
