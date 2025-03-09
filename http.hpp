#pragma once

#include <string>
#include <vector>

namespace freeblock {
struct Http {
  typedef unsigned short int HttpStatus;

  enum MethodType { GET, POST };

  MethodType methodType;
  HttpStatus status;  // http code (eg. 200, 404, 500)
  std::string url;

 public:
  Http(std::string path);
  Http(std::string path,
       std::vector<std::pair<std::string, std::string>> request);

  void start();
  void stop();

  std::string getUrl() { return this->url; };
  void setUrl(std::string url) { this->url = url; };

  MethodType getMethodType() { return this->methodType; };
  void setMethodType(MethodType methodType) { this->methodType = methodType; };
};
}  // namespace freeblock
