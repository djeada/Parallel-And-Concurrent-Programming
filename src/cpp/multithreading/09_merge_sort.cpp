#include <algorithm>
#include <chrono>
#include <iostream>
#include <iterator>
#include <random>
#include <thread>
#include <vector>

const int numThreads =
    10; // Change this value to control the number of threads used

void merge(std::vector<int> &arr, int left, int mid, int right) {
  int n1 = mid - left + 1;
  int n2 = right - mid;

  std::vector<int> L(n1);
  std::vector<int> R(n2);

  std::copy(arr.begin() + left, arr.begin() + mid + 1, L.begin());
  std::copy(arr.begin() + mid + 1, arr.begin() + right + 1, R.begin());

  int i = 0, j = 0, k = left;
  while (i < n1 && j < n2) {
    if (L[i] <= R[j]) {
      arr[k] = L[i];
      i++;
    } else {
      arr[k] = R[j];
      j++;
    }
    k++;
  }

  while (i < n1) {
    arr[k] = L[i];
    i++;
    k++;
  }

  while (j < n2) {
    arr[k] = R[j];
    j++;
    k++;
  }
}

void mergeSort(std::vector<int> &arr, int left, int right, int depth = 0) {
  if (left < right) {
    int mid = left + (right - left) / 2;
    if (depth < numThreads) {
      std::thread leftThread(mergeSort, std::ref(arr), left, mid, depth + 1);
      std::thread rightThread(mergeSort, std::ref(arr), mid + 1, right,
                              depth + 1);
      leftThread.join();
      rightThread.join();
    } else {
      mergeSort(arr, left, mid, depth + 1);
      mergeSort(arr, mid + 1, right, depth + 1);
    }
    merge(arr, left, mid, right);
  }
}

int main() {
  int n = 100000;
  std::vector<int> arr(n);
  std::iota(arr.begin(), arr.end(), 1);
  std::shuffle(arr.begin(), arr.end(), std::mt19937{std::random_device{}()});

  std::vector<int> arrCopy = arr;

  auto start = std::chrono::high_resolution_clock::now();
  mergeSort(arr, 0, arr.size() - 1);
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration = end - start;

  std::cout << "Threaded merge sort took: " << duration.count() << " seconds"
            << std::endl;

  start = std::chrono::high_resolution_clock::now();
  std::sort(arrCopy.begin(), arrCopy.end());
  end = std::chrono::high_resolution_clock::now();
  duration = end - start;

  std::cout << "Non-threaded merge sort took: " << duration.count()
            << " seconds" << std::endl;

  return 0;
}
