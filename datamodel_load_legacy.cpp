#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>

#include "block.hpp"
#include "datamodel.hpp"
#include "instance.hpp"
#include "model.hpp"
#include "workspace.hpp"
namespace freeblock {
static void parseNode(DataModel *dm, Instance *instance,
                      rapidxml::xml_node<> *node) {
  if (strcmp(node->name(), "Item") == 0) {
    std::string className = node->first_attribute("class")->value();
    if (className == "Workspace" || className == "Model") {
      Instance *toParent = instance;
      if (className == "Workspace") {
        toParent = instance->getService<WorkspaceInstance>();
      } else if (className == "Model") {
        toParent = instance->createChild<ModelInstance>();
      }
      for (rapidxml::xml_node<> *child = node->first_node(); child;
           child = child->next_sibling()) {
        parseNode(dm, toParent, child);
      }
    } else if (className == "Part") {
      BlockInstance *block = instance->createChild<BlockInstance>();
      rapidxml::xml_node<> *props = node->first_node("Properties");
      for (rapidxml::xml_node<> *prop = props->first_node(); prop;
           prop = prop->next_sibling()) {
        std::string pname = prop->first_attribute("name")->value();
        if (pname == std::string("CFrame")) {
          glm::vec3 v = glm::vec3(0.0, 0.0, 0.0);
          glm::mat3 b = glm::mat3(1.0);

          v.x = std::atof(prop->first_node("X")->value());
          v.y = std::atof(prop->first_node("Y")->value());
          v.z = std::atof(prop->first_node("Z")->value());

          b[0][0] = std::atof(prop->first_node("R00")->value());
          b[0][1] = std::atof(prop->first_node("R01")->value());
          b[0][2] = std::atof(prop->first_node("R02")->value());

          b[1][0] = std::atof(prop->first_node("R10")->value());
          b[1][1] = std::atof(prop->first_node("R11")->value());
          b[1][2] = std::atof(prop->first_node("R12")->value());

          b[2][0] = std::atof(prop->first_node("R20")->value());
          b[2][1] = std::atof(prop->first_node("R21")->value());
          b[2][2] = std::atof(prop->first_node("R22")->value());

          block->setPosition(v);
          block->setBasis(b);
        } else if (pname == std::string("Size") ||
                   pname == std::string("size")) {
          glm::vec3 v = glm::vec3(0.0, 0.0, 0.0);

          v.x = std::atof(prop->first_node("X")->value());
          v.y = std::atof(prop->first_node("Y")->value());
          v.z = std::atof(prop->first_node("Z")->value());

          block->setSize(v);
        }
      }
    }
  }
}

void DataModel::loadLegacyMap(const char *path) {
  rapidxml::file<> file(path);
  rapidxml::xml_document<> doc;
  doc.parse<0>(file.data());

  rapidxml::xml_node<> *root = doc.first_node("roblox");
  for (rapidxml::xml_node<> *node = root->first_node(); node;
       node = node->next_sibling()) {
    parseNode(this, getRoot(), node);
  }
}
}  // namespace freeblock
