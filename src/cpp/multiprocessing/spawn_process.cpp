/**
 * C++20 + POSIX: launch another program with posix_spawnp.
 * Prefer this path when no custom child work is needed between fork and exec.
 * File actions own stdout redirection; pipe endpoints close on scope exit.
 * exec_process.cpp keeps the lower-level fork/exec mechanics for comparison.
 * Compile: g++ -std=c++20 -pthread spawn_process.cpp
 */
#include "../common/posix.hpp"

#include <array>
#include <iostream>
#include <spawn.h>
#include <string>

extern char** environ;

class SpawnActions {
public:
    SpawnActions() { check(posix_spawn_file_actions_init(&actions_)); }
    ~SpawnActions() { posix_spawn_file_actions_destroy(&actions_); }
    SpawnActions(const SpawnActions&) = delete;
    SpawnActions& operator=(const SpawnActions&) = delete;
    void redirect(int from, int to) { check(posix_spawn_file_actions_adddup2(&actions_, from, to)); }
    const posix_spawn_file_actions_t* get() const noexcept { return &actions_; }
    static void check(int code) {
        if (code != 0) throw std::system_error(code, std::generic_category(), "posix_spawn");
    }

private:
    posix_spawn_file_actions_t actions_;
};

int main(int argc, char* argv[]) try {
    posix_example::Pipe output;
    SpawnActions actions;
    actions.redirect(output.writer.get(), STDOUT_FILENO);
    char command[] = "echo";
    char message[] = "Hello from posix_spawn";
    std::array<char*, 3> arguments{command, message, nullptr};
    // An optional command name also demonstrates spawn failures without a fork.
    if (argc > 1) arguments[0] = argv[1];
    pid_t child = -1;
    SpawnActions::check(posix_spawnp(&child, arguments[0], actions.get(), nullptr,
                                   arguments.data(), environ));
    output.writer.reset();
    std::string text;
    std::array<char, 256> buffer{};
    try {
        while (true) {
            const auto count = read(output.reader.get(), buffer.data(), buffer.size());
            if (count < 0 && errno == EINTR) continue;
            if (count < 0) posix_example::fail("read child output");
            if (count == 0) break;
            text.append(buffer.data(), static_cast<std::size_t>(count));
        }
    } catch (...) {
        kill(child, SIGTERM);
        posix_example::wait_for(child);
        throw;
    }
    const int status = posix_example::wait_for(child);
    std::cout << text;
    if (!posix_example::succeeded(status)) return 1;
    std::cout << "Child exited successfully\n";
    return 0;
} catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
}
