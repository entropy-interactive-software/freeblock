#pragma once
#include <functional>
#include <map>
#include <string>
#include <typeinfo>
#include <vector>

namespace freeblock::reflection {
class Described {
 public:
};

class Property {
 public:
};

class Reflection {
  friend class PropDesc;

  std::map<std::type_info, std::map<std::string, Property>> propertyMap;
  void addProperty(std::type_info, Property p);

 public:
  static Reflection* singleton();

  template <typename T>
  static std::map<std::string, Property> getDesc() {
    Reflection* t = singleton();
    auto it = t->propertyMap.find(typeid(T));
    if (it != t->propertyMap.end()) {
      return it->second;
    }
  }
};
};  // namespace freeblock::reflection
