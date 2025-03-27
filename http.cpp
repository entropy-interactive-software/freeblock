#include "http.hpp"

#include <curl/curl.h>

#include "settings.hpp"

namespace freeblock {
static rdm::CVar http_baseurl("http_baseurl", "https://endoh.ca");

Http::Http(std::string path) {
  this->setUrl(path);
  this->setMethodType(MethodType::GET);
};

// https://stackoverflow.com/questions/44994203/how-to-get-the-http-response-string-using-curl-in-c
size_t Http::writeCallBack(char *contents, size_t size, size_t nmemb,
                           void *userp) {
  ((std::string *)userp)->append((char *)contents, size * nmemb);
  return size * nmemb;
}

Http::Http(std::string path,
           std::vector<std::pair<std::string, std::string>> request) {
  this->setMethodType(MethodType::POST);
  this->setUrl(path);

  // todo: add request body stuff
}

void Http::start() {
  m.lock();

  // winsock
  curl_global_init(CURL_GLOBAL_ALL);

  CURL *curl = curl_easy_init();
  CURLcode res;
  if (curl) {
    // https://stackoverflow.com/questions/30098087/is-libcurl-really-thread-safe
    std::string buff;
    HttpStatus status;

    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(curl, CURLOPT_URL, this->getUrl().c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallBack);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buff);

    res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
    this->setHttpStatus(status);

    if (res != CURLE_OK) {
      rdm::Log::printf(rdm::LOG_ERROR,
                       "curl_easy_perform() request for %s failed: %s",
                       this->getUrl().c_str(), curl_easy_strerror(res));
      return;
    }

    this->setResponse(response);

    // cleanup
    curl_easy_cleanup(curl);
  }

  m.unlock();
}

}  // namespace freeblock
