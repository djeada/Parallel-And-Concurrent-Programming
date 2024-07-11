#include <thread>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <algorithm>
#include <iostream>

// Function declarations
void merge(int *array, unsigned int left, unsigned int mid, unsigned int right);
void sequential_merge_sort(int *array, unsigned int left, unsigned int right);
void parallel_merge_sort(int *array, unsigned int left, unsigned int right, unsigned int depth = 0);
void evaluate_performance(int num_elements, int num_eval_runs);

// Sequential implementation of merge sort
void sequential_merge_sort(int *array, unsigned int left, unsigned int right) {
    if (left < right) {
        unsigned int mid = (left + right) / 2; // find the middle point
        sequential_merge_sort(array, left, mid); // sort the left half
        sequential_merge_sort(array, mid + 1, right); // sort the right half
        merge(array, left, mid, right); // merge the two sorted halves
    }
}

// Parallel implementation of merge sort
void parallel_merge_sort(int *array, unsigned int left, unsigned int right, unsigned int depth = 0) {
    if (depth >= std::log(std::thread::hardware_concurrency())) {
        sequential_merge_sort(array, left, right);
    } else {
        unsigned int mid = (left + right) / 2;
        std::thread left_thread = std::thread(parallel_merge_sort, array, left, mid, depth + 1);
        parallel_merge_sort(array, mid + 1, right, depth + 1);
        left_thread.join();
        merge(array, left, mid, right);
    }
}

// Helper function to merge two sorted subarrays array[left..mid] and array[mid+1..right] into array
void merge(int *array, unsigned int left, unsigned int mid, unsigned int right) {
    unsigned int num_left = mid - left + 1; // number of elements in left subarray
    unsigned int num_right = right - mid; // number of elements in right subarray
    
    // copy data into temporary left and right subarrays to be merged
    int array_left[num_left], array_right[num_right];
    std::copy(&array[left], &array[mid + 1], array_left);
    std::copy(&array[mid + 1], &array[right + 1], array_right);
    
    // initialize indices for array_left, array_right, and input subarrays
    unsigned int index_left = 0;    // index to get elements from array_left
    unsigned int index_right = 0;    // index to get elements from array_right
    unsigned int index_insert = left; // index to insert elements into input array
    
    // merge temporary subarrays into original input array
    while ((index_left < num_left) || (index_right < num_right)) {
        if ((index_left < num_left) && (index_right < num_right)) {
            if (array_left[index_left] <= array_right[index_right]) {
                array[index_insert] = array_left[index_left];
                index_left++;
            } else {
                array[index_insert] = array_right[index_right];
                index_right++;
            }
        }
        // copy any remaining elements of array_left into array
        else if (index_left < num_left) {
            array[index_insert] = array_left[index_left];
            index_left++;
        }
        // copy any remaining elements of array_right into array
        else if (index_right < num_right) {
            array[index_insert] = array_right[index_right];
            index_right++;
        }
        index_insert++;
    }
}

// Function to evaluate and compare the performance of sequential and parallel implementations
void evaluate_performance(int num_elements, int num_eval_runs) {
    int original_array[num_elements], sequential_result[num_elements], parallel_result[num_elements];
    for (int i = 0; i < num_elements; i++) {
        original_array[i] = rand();
    }

    std::cout << "Evaluating Sequential Implementation..." << std::endl;
    std::chrono::duration<double> sequential_time(0);
    std::copy(&original_array[0], &original_array[num_elements], sequential_result);
    sequential_merge_sort(sequential_result, 0, num_elements - 1); // "warm up"    
    for (int i = 0; i < num_eval_runs; i++) {
        std::copy(&original_array[0], &original_array[num_elements], sequential_result); // reset result array
        auto start_time = std::chrono::high_resolution_clock::now();
        sequential_merge_sort(sequential_result, 0, num_elements - 1);
        sequential_time += std::chrono::high_resolution_clock::now() - start_time;
    }
    sequential_time /= num_eval_runs;
    
    std::cout << "Evaluating Parallel Implementation..." << std::endl;
    std::chrono::duration<double> parallel_time(0);
    std::copy(&original_array[0], &original_array[num_elements], parallel_result);
    parallel_merge_sort(parallel_result, 0, num_elements - 1); // "warm up"
    for (int i = 0; i < num_eval_runs; i++) {
        std::copy(&original_array[0], &original_array[num_elements], parallel_result); // reset result array
        auto start_time = std::chrono::high_resolution_clock::now();
        parallel_merge_sort(parallel_result, 0, num_elements - 1);
        parallel_time += std::chrono::high_resolution_clock::now() - start_time;
    }
    parallel_time /= num_eval_runs;
    
    // verify sequential and parallel results are the same
    for (int i = 0; i < num_elements; i++) {
        if (sequential_result[i] != parallel_result[i]) {
            std::cerr << "ERROR: Result mismatch at index " << i << "!" << std::endl;
            return;
        }
    }
    std::cout << "Average Sequential Time: " << sequential_time.count() * 1000 << " ms" << std::endl;
    std::cout << "  Average Parallel Time: " << parallel_time.count() * 1000 << " ms" << std::endl;
    std::cout << "Speedup: " << sequential_time / parallel_time << std::endl;
    std::cout << "Efficiency: " << 100 * (sequential_time / parallel_time) / std::thread::hardware_concurrency() << "%" << std::endl;
}

int main() {
    const int NUM_EVAL_RUNS = 100;
    const int NUM_ELEMENTS = 1000; // number of elements to sort
    
    evaluate_performance(NUM_ELEMENTS, NUM_EVAL_RUNS);

    return 0;
}
