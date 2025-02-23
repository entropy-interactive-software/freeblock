#pragma once
#include <functional>
#include <map>
#include <string>
#include <typeinfo>
#include <vector>
namespace freeblock {
class Instance;
};

namespace freeblock::reflection {

class Reflection {
  friend class PropDesc;
  struct PDesc {
    std::string name;
  };

  std::map<std::type_info, std::map<std::string, PDesc>> propDescs;

  void addPropDesc(std::type_info, PDesc d);

 public:
  static Reflection* singleton();
};

class PropDesc {
 public:
  template <typename T, typename V>
  PropDesc(const char* name, std::function<V()> getter,
           std::function<void(V)> setter) {
    static_assert(std::is_base_of<Instance, T>::value,
                  "T must inherit from Instance");
  }
};
};  // namespace freeblock::reflection
