/**
 * C++20 + libcurl: sequential and bounded asynchronous image downloads.
 * This network-I/O comparison uses a multi event loop rather than OS workers.
 * Supply your own URLs; defaults are small public sample images.
 * Build: g++ -std=c++20 download_images.cpp -o download_images -lcurl
 */
#include "../common/curl_support.hpp"

#include <chrono>
#include <iostream>
#include <span>
#include <string>
#include <vector>

std::size_t benchmark(std::span<const std::string> urls, std::size_t concurrency) {
    const auto start = std::chrono::steady_clock::now();
    const auto responses = curl_example::fetch_many(urls, concurrency);
    std::size_t bytes = 0;
    for (const auto& response : responses) {
        curl_example::check(response.outcome);
        bytes += response.body.size();
    }
    const std::chrono::duration<double> elapsed = std::chrono::steady_clock::now() - start;
    std::cout << (concurrency == 1 ? "Sequential" : "Asynchronous")
              << ": " << bytes << " bytes in " << elapsed.count() << " seconds\n";
    return bytes;
}

int main(int argc, char* argv[]) try {
    curl_example::Environment curl;
    const std::vector<std::string> urls = argc > 1
        ? std::vector<std::string>(argv + 1, argv + argc)
        : std::vector<std::string>{"https://httpbin.org/image/png", "https://httpbin.org/image/jpeg"};
    benchmark(urls, 1);
    benchmark(urls, 4);
    // Network and cache conditions vary; a speedup is not guaranteed.
    return 0;
} catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
}
