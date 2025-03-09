#include "http.hpp"

#include "settings.hpp"

namespace freeblock {
static rdm::CVar http_baseurl("http_baseurl", "https://endoh.ca");

Http::Http(std::string path) {
  this->setUrl(path);
  this->setMethodType(MethodType::GET);
};

Http::Http(std::string path,
           std::vector<std::pair<std::string, std::string>> request) {}

}  // namespace freeblock
