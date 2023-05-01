#include <chrono>
#include <curl/curl.h>
#include <future>
#include <iostream>
#include <string>
#include <vector>

std::vector<std::string> URLS = {
    "https://httpbin.org/get",
    "https://api.github.com",
    "https://www.example.com",
};

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  ((std::string *)userp)->append((char *)contents, size * nmemb);
  return size * nmemb;
}

std::string fetch_sync(const std::string &url) {
  CURL *curl;
  CURLcode res;
  std::string readBuffer;

  curl_global_init(CURL_GLOBAL_DEFAULT);
  curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
  }
  curl_global_cleanup();

  std::cout << "Synchronously fetched content from " << url << std::endl;
  return readBuffer;
}

std::vector<std::string> fetch_all_sync(const std::vector<std::string> &urls) {
  std::vector<std::string> contents;
  for (const auto &url : urls) {
    contents.push_back(fetch_sync(url));
  }
  return contents;
}

std::string fetch_async(const std::string &url) { return fetch_sync(url); }

std::vector<std::string> fetch_all_async(const std::vector<std::string> &urls) {
  std::vector<std::future<std::string>> futures;
  for (const auto &url : urls) {
    futures.push_back(std::async(std::launch::async, fetch_async, url));
  }

  std::vector<std::string> contents;
  for (auto &future : futures) {
    contents.push_back(future.get());
  }
  return contents;
}

int main() {
  auto start_time_sync = std::chrono::steady_clock::now();
  auto contents_sync = fetch_all_sync(URLS);
  auto elapsed_time_sync =
      std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::steady_clock::now() - start_time_sync)
          .count();
  std::cout << "\nSynchronous execution took " << elapsed_time_sync / 1000.0
            << " seconds.\n";

  auto start_time_async = std::chrono::steady_clock::now();
  auto contents_async = fetch_all_async(URLS);
  auto elapsed_time_async =
      std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::steady_clock::now() - start_time_async)
          .count();
  std::cout << "\nAsynchronous execution took " << elapsed_time_async / 1000.0
            << " seconds.\n";

  return 0;
}
