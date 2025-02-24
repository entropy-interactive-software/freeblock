#pragma once
#include <map>

#include "filesystem.hpp"

namespace freeblock {
//
class HttpRequestApi {
  enum MethodType {
    GET,
    // POST,
    // PUT,
  };

  enum RequestSchema { HTTP, HTTPS };
  enum RequestStatus { FAILED, SUCCESS, IN_PROGRESS };

  typedef unsigned int StatusCode;

  StatusCode status;
  std::string url;  // full url (eg. https://google.com/abcdefg?asdasd)

  int port;
  RequestSchema schema;  // https, http
  std::string domain;    // test.com => https://test.com/
  std::vector<std::string>
      subdomain;  // ['clientsettings', 'api'] => clientsettings.api
  std::map<std::string, std::string> variables;  // ['a' => 'b'] = ?a=b
  std::string path;  // peepee/body => https://test.com/peepee/body

  common::OptionalData response;

 public:
  HttpRequestApi(std::string url);

  void sendRequest();
  void cancelRequest();

  common::OptionalData getIO();
};
}  // namespace freeblock
