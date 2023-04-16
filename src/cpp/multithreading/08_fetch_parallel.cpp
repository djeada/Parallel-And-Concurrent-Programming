#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <curl/curl.h>

std::vector<std::string> urls = {
    "https://www.example.com",
    "https://www.example.org",
    "https://www.example.net",
    "https://www.example.edu",
};

std::size_t writeFunction(void* contents, std::size_t size, std::size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::pair<std::string, std::size_t> fetchUrl(const std::string& url) {
    CURL* curl = curl_easy_init();
    std::string fetchedContent;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &fetchedContent);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }

    return {url, fetchedContent.length()};
}

void concurrentFetch(const std::string& url) {
    auto [fetchedUrl, contentLength] = fetchUrl(url);
    std::cout << "Concurrent - URL: " << fetchedUrl << ", Content Length: " << contentLength << std::endl;
}

int main() {
    curl_global_init(CURL_GLOBAL_DEFAULT);

    // Non-concurrent fetching
    auto nonConcurrentStartTime = std::chrono::steady_clock::now();
    for (const auto& url : urls) {
        auto [fetchedUrl, contentLength] = fetchUrl(url);
        std::cout << "Non-concurrent - URL: " << fetchedUrl << ", Content Length: " << contentLength << std::endl;
    }
    auto nonConcurrentEndTime = std::chrono::steady_clock::now();
    auto nonConcurrentElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(nonConcurrentEndTime - nonConcurrentStartTime).count();
    std::cout << "Non-concurrent time taken: " << nonConcurrentElapsed / 1000.0 << " seconds" << std::endl;

    // Concurrent fetching
    auto concurrentStartTime = std::chrono::steady_clock::now();
    std::vector<std::thread> threads;
    for (const auto& url : urls) {
        threads.emplace_back(concurrentFetch, url);
    }

    for (auto& thread : threads) {
        thread.join();
    }
    auto concurrentEndTime = std::chrono::steady_clock::now();
    auto concurrentElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(concurrentEndTime - concurrentStartTime).count();
    std::cout << "Concurrent time taken: " << concurrentElapsed / 1000.0 << " seconds" << std::endl;

    curl_global_cleanup();

    return 0;
}
