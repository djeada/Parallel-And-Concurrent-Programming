/**
 * C++20 + libcurl: bounded asynchronous HTTP on one event-loop thread.
 * curl_multi_perform/poll progresses transfers without a thread per URL.
 * Scoped easy/multi handles clean up on success and failure.
 * Build: g++ -std=c++20 parallel_fetch.cpp -o parallel_fetch -lcurl
 * Run: ./parallel_fetch [url ...]
 */
#include "../common/curl_support.hpp"

#include <chrono>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char* argv[]) try {
    curl_example::Environment curl;
    const std::vector<std::string> urls = argc > 1
        ? std::vector<std::string>(argv + 1, argv + argc)
        : std::vector<std::string>{"https://example.com", "https://www.iana.org"};
    const auto start = std::chrono::steady_clock::now();
    const auto responses = curl_example::fetch_many(urls, 4);
    bool success = true;
    for (std::size_t index = 0; index < urls.size(); ++index) {
        const auto& response = responses[index];
        if (response.outcome != CURLE_OK) {
            std::cerr << "Failed: " << urls[index] << ": " << curl_easy_strerror(response.outcome) << '\n';
            success = false;
        } else {
            std::cout << "Fetched: " << urls[index] << " (" << response.body.size() << " bytes)\n";
        }
    }
    const std::chrono::duration<double> elapsed = std::chrono::steady_clock::now() - start;
    std::cout << "Asynchronous: " << elapsed.count() << "s\n";
    return success ? 0 : 1;
} catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
}
