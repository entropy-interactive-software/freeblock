#pragma once

#include <curl/curl.h>

#include <mutex>
#include <string>
#include <vector>

namespace freeblock {
struct Http {
  typedef unsigned short int HttpStatus;

  enum MethodType { GET, POST };

  MethodType methodType;
  HttpStatus status;  // http code (eg. 200, 404, 500)
  std::string url;
  std::mutex m;  // only one thread can call a start
  std::string response;

  // CURL *curl;
  // CURLcode res;

 public:
  Http(std::string path);
  Http(std::string path,
       std::vector<std::pair<std::string, std::string>> request);

  static size_t writeCallBack(char *contents, size_t size, size_t nmemb,
                              void *userp);

  void start();
  void stop();

  std::string getUrl() { return this->url; };
  void setUrl(std::string url) { this->url = url; };

  MethodType getMethodType() { return this->methodType; };
  void setMethodType(MethodType methodType) { this->methodType = methodType; };

  HttpStatus getHttpStatus() { return this->status; };
  void setHttpStatus(HttpStatus status) { this->status = status; };

  std::string getResponse() { return this->response; };
  void setResponse(std::string response) { this->response = response; };
};
}  // namespace freeblock
