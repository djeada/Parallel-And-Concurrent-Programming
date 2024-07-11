#include <thread>
#include <chrono>

void assistant_james() {
    printf("James started & waiting for documents to process...\n");
    std::this_thread::sleep_for(std::chrono::seconds(3));
    printf("James is done processing documents.\n");
}

int main() {
    printf("Manager requests James's help.\n");
    std::thread james(assistant_james);
    printf("  James is joinable? %s\n", james.joinable() ? "true" : "false");

    printf("Manager continues preparing the report.\n");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    printf("  James is joinable? %s\n", james.joinable() ? "true" : "false");

    printf("Manager patiently waits for James to finish and join...\n");
    james.join();
    printf("  James is joinable? %s\n", james.joinable() ? "true" : "false");

    printf("Manager and James are both done!\n");
    
    return 0;
}
