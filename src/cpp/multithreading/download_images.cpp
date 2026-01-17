/**
 * Parallel Image Download with std::async
 *
 * This example demonstrates parallel I/O operations using std::async,
 * comparing sequential vs parallel download performance.
 *
 * Key concepts:
 * - std::async launches tasks that may run in parallel
 * - std::future allows retrieving results from async operations
 * - I/O-bound tasks (like network requests) benefit greatly from parallelism
 * - Speedup depends on network latency, not CPU cores
 *
 * Build: g++ -std=c++17 -o download_images download_images.cpp -lcurl
 */

#include <chrono>
#include <curl/curl.h>
#include <future>
#include <iostream>
#include <list>
#include <string>
#include <thread>

// Callback to discard downloaded data (we only measure transfer size)
size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    return size * nmemb;
}

// Download a single image and return its size
size_t download_image(int image_num) {
    std::string url = "https://example.com/image_" +
                      std::to_string((image_num % 50) + 1) + ".jpg";

    curl_off_t num_bytes = 0;
    CURL* curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

        CURLcode res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD_T, &num_bytes);
        }
        curl_easy_cleanup(curl);
    }

    return static_cast<size_t>(num_bytes);
}

// Sequential: download one at a time
size_t sequential_download(int num_images) {
    size_t total_bytes = 0;
    for (int i = 1; i <= num_images; ++i) {
        total_bytes += download_image(i);
    }
    return total_bytes;
}

// Parallel: download all concurrently using std::async
size_t parallel_download(int num_images) {
    std::list<std::future<size_t>> futures;

    // Launch all downloads in parallel
    for (int i = 1; i <= num_images; ++i) {
        futures.push_back(std::async(std::launch::async, download_image, i));
    }

    // Collect results
    size_t total_bytes = 0;
    for (auto& f : futures) {
        total_bytes += f.get();
    }
    return total_bytes;
}

void benchmark(int num_images, int num_runs) {
    using namespace std::chrono;

    // Benchmark sequential
    std::cout << "Evaluating Sequential Download...\n";
    duration<double> seq_time(0);
    size_t seq_result = sequential_download(num_images);  // Warm up

    for (int i = 0; i < num_runs; ++i) {
        auto start = high_resolution_clock::now();
        sequential_download(num_images);
        seq_time += high_resolution_clock::now() - start;
    }
    seq_time /= num_runs;

    // Benchmark parallel
    std::cout << "Evaluating Parallel Download...\n";
    duration<double> par_time(0);
    size_t par_result = parallel_download(num_images);  // Warm up

    for (int i = 0; i < num_runs; ++i) {
        auto start = high_resolution_clock::now();
        parallel_download(num_images);
        par_time += high_resolution_clock::now() - start;
    }
    par_time /= num_runs;

    // Results
    std::cout << "\nResults:\n";
    std::cout << "  Sequential: " << seq_time.count() * 1000 << " ms\n";
    std::cout << "  Parallel:   " << par_time.count() * 1000 << " ms\n";
    std::cout << "  Speedup:    " << seq_time.count() / par_time.count() << "x\n";
}

int main() {
    curl_global_init(CURL_GLOBAL_DEFAULT);

    benchmark(50, 3);

    curl_global_cleanup();
    return 0;
}
