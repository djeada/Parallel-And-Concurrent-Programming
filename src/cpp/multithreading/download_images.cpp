#include <thread>
#include <cstring>
#include <curl/curl.h> 
#include <future>
#include <list>
#include <chrono>
#include <iostream>

// Function declarations
size_t download_image(int image_num);
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp);
size_t sequential_image_downloader(int num_images);
size_t parallel_image_downloader(int num_images);
void evaluate_performance(int num_images, int num_eval_runs);

// Sequential implementation of image downloader
size_t sequential_image_downloader(int num_images) {
    size_t total_bytes = 0;
    for (int i = 1; i <= num_images; i++) {
        total_bytes += download_image(i);
    }
    return total_bytes;
}

// Parallel implementation of image downloader using std::async
size_t parallel_image_downloader(int num_images) {
    size_t total_bytes = 0;
    std::list<std::future<size_t>> download_futures;
    for (int i = 1; i <= num_images; i++) {
        download_futures.push_back(std::async(std::launch::async, download_image, i));
    }
    for (auto& future : download_futures) {
        total_bytes += future.get();
    }
    return total_bytes;
}

// Helper function to download a single image and return the size in bytes
size_t download_image(int image_num) {
    char url[256];
    sprintf(url, "https://example.com/image_%d.jpg", ((image_num % 50) + 1));

    CURLcode res;
    curl_off_t num_bytes = 0;
    CURL *curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        res = curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD_T, &num_bytes);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_getinfo() failed: " << curl_easy_strerror(res) << std::endl;
        }
        curl_easy_cleanup(curl);
    }
    return num_bytes;
}

// Callback function for cURL to handle data received from the server
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    return size * nmemb;
}

// Function to evaluate and compare the performance of sequential and parallel implementations
void evaluate_performance(int num_images, int num_eval_runs) {
    std::cout << "Evaluating Sequential Implementation..." << std::endl;
    std::chrono::duration<double> sequential_time(0);
    size_t sequential_result = sequential_image_downloader(num_images); // "warm up"
    for (int i = 0; i < num_eval_runs; i++) {
        auto startTime = std::chrono::high_resolution_clock::now();
        sequential_image_downloader(num_images);
        sequential_time += std::chrono::high_resolution_clock::now() - startTime;
    }
    sequential_time /= num_eval_runs;

    std::cout << "Evaluating Parallel Implementation..." << std::endl;
    std::chrono::duration<double> parallel_time(0);
    size_t parallel_result = parallel_image_downloader(num_images); // "warm up"
    for (int i = 0; i < num_eval_runs; i++) {
        auto startTime = std::chrono::high_resolution_clock::now();
        parallel_image_downloader(num_images);
        parallel_time += std::chrono::high_resolution_clock::now() - startTime;
    }
    parallel_time /= num_eval_runs;

    // Display sequential and parallel results for comparison
    if (sequential_result != parallel_result) {
        std::cerr << "ERROR: Result mismatch!" << std::endl
                  << "  Sequential Result = " << sequential_result << std::endl
                  << "  Parallel Result = " << parallel_result << std::endl;
    }
    std::cout << "Average Sequential Time: " << sequential_time.count() * 1000 << " ms" << std::endl;
    std::cout << "  Average Parallel Time: " << parallel_time.count() * 1000 << " ms" << std::endl;
    std::cout << "Speedup: " << sequential_time / parallel_time << std::endl;
    std::cout << "Efficiency: " << 100 * (sequential_time / parallel_time) / std::thread::hardware_concurrency() << "%" << std::endl;
}

int main() {
    const int NUM_EVAL_RUNS = 3; // Number of evaluation runs for averaging the performance
    const int NUM_IMAGES = 50;   // Number of images to download

    evaluate_performance(NUM_IMAGES, NUM_EVAL_RUNS);

    return 0;
}
