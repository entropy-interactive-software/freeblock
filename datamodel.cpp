#pragma once
#include "datamodel.hpp"

#include <random>

#include "instance.hpp"
#include "script_context.hpp"
namespace freeblock {
DataModel::DataModel(rdm::World* world) {
  this->world = world;
  root = new Instance(this);
}

DataModel::~DataModel() { delete root; }

// STOLEN FROM https://stackoverflow.com/a/60198074
static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_int_distribution<> dis(0, 15);
static std::uniform_int_distribution<> dis2(8, 11);

std::string generate_uuid_v4() {
  std::stringstream ss;
  int i;
  ss << std::hex;
  for (i = 0; i < 8; i++) {
    ss << dis(gen);
  }
  ss << "-";
  for (i = 0; i < 4; i++) {
    ss << dis(gen);
  }
  ss << "-4";
  for (i = 0; i < 3; i++) {
    ss << dis(gen);
  }
  ss << "-";
  ss << dis2(gen);
  for (i = 0; i < 3; i++) {
    ss << dis(gen);
  }
  ss << "-";
  for (i = 0; i < 12; i++) {
    ss << dis(gen);
  };
  return ss.str();
}

InstanceUUID DataModel::newInstance(Instance* instance) {
  InstanceUUID uuid = generate_uuid_v4();
  instances[uuid] = instance;
  return uuid;
}

Instance* DataModel::getInstanceByUUID(InstanceUUID uuid) {
  auto it = instances.find(uuid);
  if (it != instances.end()) {
    return it->second;
  } else {
    return NULL;
  }
}
};  // namespace freeblock
