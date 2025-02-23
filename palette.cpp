#include "palette.hpp"

#include <glm/fwd.hpp>

namespace freeblock {

static glm::vec3 palette[] = {
    glm::vec3(0, 0, 0),        // black
    glm::vec3(255, 255, 255),  // white
    glm::vec3(52, 58, 235)     // blue
};

glm::vec3 Palette::blockColorToColor(BlockColor color) {
  return palette[color];
};

BlockColor Palette::colorToBlockColor(glm::vec3 color) {
  // pee

  return BlockColor(0);
}
};  // namespace freeblock
