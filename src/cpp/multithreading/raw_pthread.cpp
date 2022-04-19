#include <iostream>
#include <pthread.h>

struct SumData {
    long long start;
    long long end;
    long long result;
};

void *sum(void *data) {
    SumData *sumData = (SumData *)data;

    for (long long i = sumData->start; i < sumData->end; i++) {
        sumData->result += i;
    }
    
    pthread_exit(NULL);
}

int main() {
    long long start = 0;
    long long end = 1000000;
    int numThreads = 10;

    pthread_t threads[numThreads];
    SumData sumData[numThreads];

    for (int i = 0; i < numThreads; i++) {
        sumData[i].start = start + (end - start) / numThreads * i;
        sumData[i].end = start + (end - start) / numThreads * (i + 1);
        sumData[i].result = 0;
        pthread_create(&threads[i], NULL, sum, (void *)&sumData[i]);
    }

    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    long long result = 0;
    for (int i = 0; i < numThreads; i++) {
        result += sumData[i].result;
    }

    std::cout << result << std::endl;
    return 0;
}
