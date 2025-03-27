#pragma once
#include <functional>
#include <glm/glm.hpp>
#include <map>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <vector>

#include "reflection_props.hpp"

namespace freeblock::reflection {
typedef std::map<std::string, Property*> PropertyList;

#define DESCRIBED                                              \
 public:                                                       \
  virtual freeblock::reflection::PropertyList getProperties(); \
                                                               \
 private:

class Described {
  int references;

 public:
  virtual ~Described() {}

  int gcGetNumReferences() { return references; }
  void gcAddReference() { references++; };
  void gcRmReference() { references--; };

  virtual std::string getParentClassName() const { return "nil"; }
  virtual std::string getClassName() const { return "Described"; }

  virtual PropertyList getProperties() {
    PropertyList pl;
    return pl;
  };
};

class Reflection {
  std::map<std::string, std::vector<Property>> properties;

  PropertyList getPropertiesForName(std::string str);

 public:
  static Reflection* singleton();

  PropertyList getProperties(Described* described);
};

#define REFLECTION_BEGIN_DESCRIBED(D)                                   \
  freeblock::reflection::PropertyList D::getProperties() {              \
    freeblock::reflection::PropertyList pl;                             \
    {                                                                   \
      freeblock::reflection::PropertyList ppl = Super::getProperties(); \
      pl.insert(ppl.begin(), ppl.end());                                \
    }
#define REFLECTION_END_DESCRIBED() \
  return pl;                       \
  }

};  // namespace freeblock::reflection
