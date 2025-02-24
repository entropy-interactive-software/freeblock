#pragma once
#include <string>

#include "filesystem.hpp"

namespace freeblock {
class ContentId {
  std::string id;  // full w/ schema (eg. asset://test.rbxl)
  std::string schema;
  std::string path;

  enum Schema {
    ASSET_ID,
  };

 public:
  ContentId(std::string contentId);
  ContentId(ContentId& contentId);
};
}  // namespace freeblock
