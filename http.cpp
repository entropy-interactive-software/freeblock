#include "http.hpp"

#include <curl/curl.h>

#include <thread>

#include "settings.hpp"

namespace freeblock {
static rdm::CVar http_baseurl("http_baseurl", "https://endoh.ca");

void Http::globalInit() { curl_global_init(CURL_GLOBAL_ALL); }

Http::Http(std::string path) {
  std::string base_url = http_baseurl.getValue();

  if (!base_url.empty() && base_url.back() == '/' && !path.empty() &&
      path.front() == '/') {
    path.erase(0, 1);
  }
  // this->setUrl(base_url + path);
  this->setUrl(path);
  this->setMethodType(MethodType::GET);
}

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
  std::scoped_lock<std::mutex> lock(m);

  // std::call_once(http_global_init, &Http::globalInit);

  CURL *curl = curl_easy_init();
  if (!curl) {
    rdm::Log::printf(rdm::LOG_ERROR, "curl_easy_init() failed");
    return;
  }

  // https://stackoverflow.com/questions/30098087/is-libcurl-really-thread-safe
  std::string buff = "";
  HttpStatus status = 0;

  curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
  curl_easy_setopt(curl, CURLOPT_URL, this->url.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &Http::writeCallBack);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buff);

  CURLcode res = curl_easy_perform(curl);

  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
  this->status = status;

  if (res != CURLE_OK) {
    rdm::Log::printf(rdm::LOG_ERROR,
                     "curl_easy_perform() request for %s failed: %s",
                     this->getUrl().c_str(), curl_easy_strerror(res));
    curl_easy_cleanup(curl);
    return;
  }

  this->status = status;
  this->response = buff;

  // cleanup
  curl_easy_cleanup(curl);

  rdm::Log::printf(rdm::LOG_INFO,
                   "curl_easy_perform() request for %s success: %s",
                   this->url.c_str(), this->response.c_str());
}

}  // namespace freeblock
