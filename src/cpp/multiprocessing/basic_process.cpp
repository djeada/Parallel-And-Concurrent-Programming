/**

 * C++20 + POSIX: scoped child ownership.

 * ChildGroup checks fork, flushes before forking, reaps owned children, and

 * reports abnormal exits. The parent in these fork examples is single-threaded.

 * For launching another program, start with spawn_process.cpp.

 * Compile: g++ -std=c++20 -pthread basic_process.cpp

 */

#include "../common/posix.hpp"

#include <iostream>

int main() try {

    std::cout << "Main process started (PID: " << getpid() << ")\n";

    posix_example::ChildGroup children(1);

    children.spawn([] {

        std::cout << "Child process started\n"

                  << "  Child PID: " << getpid() << '\n'

                  << "  Parent PID: " << getppid() << '\n'

                  << "Child process finished\n";

    });

    if (!children.wait_all()) return 1;

    std::cout << "Parent process finished\n";

    return 0;

} catch (const std::exception& error) {

    std::cerr << error.what() << '\n';

    return 1;

}