/**
 * Parallel HTTP Fetching
 *
 * This example demonstrates fetching multiple URLs in parallel
 * using std::async, comparing synchronous vs asynchronous performance.
 *
 * Key concepts:
 * - I/O-bound tasks benefit greatly from async execution
 * - Each fetch runs in its own thread (with std::launch::async)
 * - Results are collected as futures complete
 * - Speedup is proportional to the number of independent fetches
 *
 * Build: g++ -std=c++17 -o parallel_fetch parallel_fetch.cpp -lcurl
 */

#include <chrono>
#include <curl/curl.h>
#include <future>
#include <iostream>
#include <string>
#include <vector>

const std::vector<std::string> URLS = {
    "https://httpbin.org/get",
    "https://api.github.com",
    "https://www.example.com",
};

size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    static_cast<std::string*>(userp)->append(
        static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}

std::string fetch_url(const std::string& url) {
    std::string buffer;
    CURL* curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }

    std::cout << "Fetched: " << url << "\n";
    return buffer;
}

void fetch_sync() {
    auto start = std::chrono::steady_clock::now();

    for (const auto& url : URLS) {
        fetch_url(url);
    }

    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start);
    std::cout << "Synchronous: " << elapsed.count() / 1000.0 << "s\n\n";
}

void fetch_async() {
    auto start = std::chrono::steady_clock::now();

    std::vector<std::future<std::string>> futures;
    for (const auto& url : URLS) {
        futures.push_back(std::async(std::launch::async, fetch_url, url));
    }

    for (auto& f : futures) {
        f.get();
    }

    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start);
    std::cout << "Asynchronous: " << elapsed.count() / 1000.0 << "s\n";
}

int main() {
    curl_global_init(CURL_GLOBAL_DEFAULT);

    fetch_sync();
    fetch_async();

    curl_global_cleanup();
    return 0;
}
