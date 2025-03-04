#include "reflection.hpp"
namespace freeblock::reflection {
static Reflection* _singleton = 0;
Reflection* Reflection::singleton() {
  if (!_singleton) _singleton = new Reflection();
  return _singleton;
}

PropertyList Reflection::getProperties(Described* described) {
  PropertyList properties;

  return properties;
}
};  // namespace freeblock::reflection
