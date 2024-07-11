#include <thread>
#include <chrono>

void log_monitor() {
    while (true) {
        printf("Alice is monitoring the logs.\n");
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main() {
    std::thread alice(log_monitor);
    alice.detach();
    for (int i = 0; i < 3; i++) {
        printf("Bob is processing data...\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(600));
    }
    printf("Bob is done!\n");

    return 0;
}
