#pragma once
#include "datamodel.hpp"

#include <random>

#include "datamodel_described.hpp"
#include "instance.hpp"
#include "runservice.hpp"
#include "script_context.hpp"
#include "workspace.hpp"
namespace freeblock {
DataModel::DataModel(rdm::World* world) {
  this->world = world;
  root = new DataModelDescribed(this);
  root->gcAddReference();

  RunService* run = root->getService<RunService>();
  run->stop();

  // loadLegacyMap("map.rbxl");
  WorkspaceInstance* workspace = root->getService<WorkspaceInstance>();
  workspace->setInfinitePlane(true);
}

DataModel::~DataModel() { delete root; }

bool DataModel::isServer() {
  return getWorld()->getNetworkManager()->isBackend();
}

bool DataModel::isClient() {
  return !getWorld()->getNetworkManager()->isBackend();
}

void DataModel::step() {
  for (auto& i : instances) {
    i.second->step();
  }
}

void DataModel::removeInstance(InstanceUUID uuid) { instances.erase(uuid); }

// STOLEN FROM https://stackoverflow.com/a/60198074
static std::random_device rd;
static std::mt19937_64 gen(rd());
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

void DataModel::newInstanceTracked(Instance* instance, InstanceUUID uuid) {
  instances[uuid] = instance;
}

void DataModel::setInstanceUUID(InstanceUUID old, InstanceUUID newu) {
  auto it = instances.find(old);
  if (it != instances.end()) {
    Instance* i = it->second;
    instances.erase(old);
    i->setUUID(newu);
    rdm::Log::printf(rdm::LOG_DEBUG, "%s -> %s", old.c_str(), newu.c_str());
    instances[newu] = i;
  } else {
    throw std::runtime_error("Non existent uuid");
  }
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
