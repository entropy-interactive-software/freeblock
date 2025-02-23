#include "reflection.hpp"
namespace freeblock::reflection {
void Reflection::addPropDesc(PDesc* d) {}

static Reflection* _singleton = 0;
Reflection* Reflection::singleton() {
  if (!_singleton) _singleton = new Reflection();
  return _singleton;
}
};  // namespace freeblock::reflection
