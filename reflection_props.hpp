#pragma once
#include <functional>
#include <glm/glm.hpp>
#include <stdexcept>
#include <string>
extern "C" {
#include "lua/lauxlib.h"
#include "lua/lua.h"
}

namespace freeblock {
class Instance;
};

struct lua_State;

namespace freeblock::reflection {
class Described;

typedef int LuaFunctionT(lua_State*);
typedef std::function<LuaFunctionT> LuaFunction;

class Property {
 protected:
  std::string name;

 public:
  enum Type {
    String,
    Integer,
    Bool,
    Float,
    Vec3,
    Vec2,
    InstanceRef,
    Function,
    Signal
  };

  virtual bool isWriteable() { return false; }

  const char* getName() const { return name.c_str(); };
  virtual Type getType() = 0;

  virtual std::string getString(Described* described) {
    throw std::runtime_error("No string");
  }
  virtual void setString(Described* described, std::string str) {
    throw std::runtime_error("No string");
  }

  virtual int getInt(Described* described) {
    throw std::runtime_error("No int");
  }
  virtual void setInt(Described* described, int value) {
    throw std::runtime_error("No int");
  }

  virtual bool getBool(Described* described) {
    throw std::runtime_error("No bool");
  }
  virtual void setBool(Described* described, bool value) {
    throw std::runtime_error("No bool");
  }

  virtual float getFloat(Described* described) {
    throw std::runtime_error("No float");
  }
  virtual void setFloat(Described* described, float value) {
    throw std::runtime_error("No float");
  }

  virtual glm::vec3 getVec3(Described* described) {
    throw std::runtime_error("No vec3");
  }
  virtual void setVec3(Described* described, glm::vec3 value) {
    throw std::runtime_error("No vec3");
  }

  virtual Instance* getInstance(Described* described) {
    throw std::runtime_error("No instance");
  }
  virtual void setInstance(Described* described, Instance* instance) {
    throw std::runtime_error("No instance");
  }

  virtual LuaFunction getFunction() { throw std::runtime_error("No function"); }
};

template <typename T>
class PropertyString : public Property {
  std::function<void(T*, std::string)> setter;
  std::function<std::string(T*)> getter;

 public:
  typedef std::function<void(T*, std::string)> Setter;
  typedef std::function<std::string(T*)> Getter;

  virtual bool isWriteable() { return (setter != nullptr); }

  PropertyString(std::string name, Setter set, Getter get) {
    this->name = name;
    setter = set;
    getter = get;
  }

  virtual Type getType() { return String; }

  virtual std::string getString(Described* described) {
    return getter(dynamic_cast<T*>(described));
  }

  virtual void setString(Described* described, std::string str) {
    setter(dynamic_cast<T*>(described), str);
  }
};

template <typename T>
class PropertyInt : public Property {
  typedef int DataType;

  std::function<void(T*, DataType)> setter;
  std::function<DataType(T*)> getter;

 public:
  typedef std::function<void(T*, DataType)> Setter;
  typedef std::function<DataType(T*)> Getter;

  virtual bool isWriteable() { return (setter != nullptr); }

  PropertyInt(std::string name, Setter set, Getter get) {
    this->name = name;
    setter = set;
    getter = get;
  }

  virtual Type getType() { return Integer; }

  virtual DataType getInt(Described* described) {
    return getter(dynamic_cast<T*>(described));
  }

  virtual void setInt(Described* described, DataType str) {
    setter(dynamic_cast<T*>(described), str);
  }
};

template <typename T>
class PropertyBool : public Property {
  typedef bool DataType;

  std::function<void(T*, DataType)> setter;
  std::function<DataType(T*)> getter;

 public:
  typedef std::function<void(T*, DataType)> Setter;
  typedef std::function<DataType(T*)> Getter;

  virtual bool isWriteable() { return (setter != nullptr); }

  PropertyBool(std::string name, Setter set, Getter get) {
    this->name = name;
    setter = set;
    getter = get;
  }

  virtual Type getType() { return Bool; }

  virtual DataType getBool(Described* described) {
    return getter(dynamic_cast<T*>(described));
  }

  virtual void setBool(Described* described, DataType str) {
    setter(dynamic_cast<T*>(described), str);
  }
};

template <typename T>
class PropertyFloat : public Property {
  typedef float DataType;

  std::function<void(T*, DataType)> setter;
  std::function<DataType(T*)> getter;

 public:
  typedef std::function<void(T*, DataType)> Setter;
  typedef std::function<DataType(T*)> Getter;

  virtual bool isWriteable() { return (setter != nullptr); }

  PropertyFloat(std::string name, Setter set, Getter get) {
    this->name = name;
    setter = set;
    getter = get;
  }

  virtual Type getType() { return Float; }

  virtual DataType getFloat(Described* described) {
    return getter(dynamic_cast<T*>(described));
  }

  virtual void setFloat(Described* described, DataType str) {
    setter(dynamic_cast<T*>(described), str);
  }
};

template <typename T>
class PropertyVec3 : public Property {
  typedef glm::vec3 DataType;

  std::function<void(T*, DataType)> setter;
  std::function<DataType(T*)> getter;

 public:
  typedef std::function<void(T*, DataType)> Setter;
  typedef std::function<DataType(T*)> Getter;

  virtual bool isWriteable() { return (setter != nullptr); }

  PropertyVec3(std::string name, Setter set, Getter get) {
    this->name = name;
    setter = set;
    getter = get;
  }

  virtual Type getType() { return Vec3; }

  virtual DataType getVec3(Described* described) {
    return getter(dynamic_cast<T*>(described));
  }

  virtual void setVec3(Described* described, DataType str) {
    setter(dynamic_cast<T*>(described), str);
  }
};

template <typename T>
class PropertyInstance : public Property {
  typedef Instance* DataType;

  std::function<void(T*, DataType)> setter;
  std::function<DataType(T*)> getter;

 public:
  typedef std::function<void(T*, DataType)> Setter;
  typedef std::function<DataType(T*)> Getter;

  virtual bool isWriteable() { return (setter != nullptr); }

  PropertyInstance(std::string name, Setter set, Getter get) {
    this->name = name;
    setter = set;
    getter = get;
  }

  virtual Type getType() { return InstanceRef; }

  virtual DataType getInstance(Described* described) {
    return getter(dynamic_cast<T*>(described));
  }

  virtual void setInstance(Described* described, DataType str) {
    setter(dynamic_cast<T*>(described), str);
  }
};

template <typename T>
class PropertyFunction : public Property {
  typedef LuaFunction DataType;
  LuaFunction func;

 public:
  PropertyFunction(std::string name, LuaFunction func) {
    this->name = name;
    this->func = func;
  }

  virtual Type getType() { return Function; }

  DataType getFunction() { return func; }
};

#define REFLECTION_PROPERTY_STRING(T, N, Gt, St)                     \
  static freeblock::reflection::PropertyString<T> __##N(#N, St, Gt); \
  pl[#N] = &__##N;

#define REFLECTION_PROPERTY_INT(T, N, Gt, St)                     \
  static freeblock::reflection::PropertyInt<T> __##N(#N, St, Gt); \
  pl[#N] = &__##N;

#define REFLECTION_PROPERTY_BOOL(T, N, Gt, St)                     \
  static freeblock::reflection::PropertyBool<T> __##N(#N, St, Gt); \
  pl[#N] = &__##N;

#define REFLECTION_PROPERTY_FLOAT(T, N, Gt, St)                     \
  static freeblock::reflection::PropertyFloat<T> __##N(#N, St, Gt); \
  pl[#N] = &__##N;

#define REFLECTION_PROPERTY_VEC3(T, N, Gt, St)                     \
  static freeblock::reflection::PropertyVec3<T> __##N(#N, St, Gt); \
  pl[#N] = &__##N;

#define REFLECTION_PROPERTY_INSTANCE(T, N, Gt, St)                     \
  static freeblock::reflection::PropertyInstance<T> __##N(#N, St, Gt); \
  pl[#N] = &__##N;

#define REFLECTION_FUNCTION(T, N, Func)                              \
  static freeblock::reflection::PropertyFunction<T> __##N(#N, Func); \
  pl[#N] = &__##N;
};  // namespace freeblock::reflection
