/**
 * Named Pipes (FIFOs) — IPC Between Unrelated Processes
 *
 * A named pipe (FIFO) is like an anonymous pipe but has a
 * filesystem path, so any process can open it by name — no
 * parent/child relationship required.
 *
 * Key behaviours:
 *   - open() BLOCKS until BOTH ends (reader + writer) are open.
 *   - Data flows one way (write end → read end), FIFO ordering.
 *   - When the writer closes, reader gets EOF (read == 0).
 *   - Writing to a FIFO with no reader raises SIGPIPE / EPIPE.
 *   - mkfifo() creates the FIFO; unlink() removes it.
 *
 * Contrast with anonymous pipes (pipe()):
 *   - Anonymous pipes require a parent/child relationship.
 *   - Named pipes work between completely unrelated processes.
 *   - Named pipes persist in the filesystem until unlinked.
 *
 * Usage (two separate terminals):
 *   ./named_pipe reader           — opens and reads
 *   ./named_pipe writer           — opens and writes
 *
 * Or for a self-contained demo (forks both internally):
 *   ./named_pipe
 *
 * Compile:
 *   g++ -std=c++17 -o named_pipe named_pipe.cpp
 */

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

constexpr char FIFO_PATH[] = "/tmp/example_fifo";
constexpr int  NUM_MSGS    = 5;

// ── Writer: sends NUM_MSGS lines, then closes ──────────────────────────────

static void run_writer() {
    std::cout << "[writer] opening FIFO (blocks until reader opens)...\n";

    // O_WRONLY blocks here until a reader opens the other end
    int fd = open(FIFO_PATH, O_WRONLY);
    if (fd < 0) {
        perror("open(FIFO_PATH, O_WRONLY)");
        _exit(1);
    }

    std::cout << "[writer] connected\n";
    for (int i = 1; i <= NUM_MSGS; ++i) {
        std::string msg = "Message " + std::to_string(i) + "\n";
        ssize_t written = write(fd, msg.c_str(), msg.size());
        if (written < 0) {
            if (errno == EPIPE) {
                std::cerr << "[writer] reader closed early (EPIPE)\n";
            } else {
                perror("write");
            }
            break;
        }
        std::cout << "[writer] sent: " << msg;
        usleep(200'000);  // 200 ms between messages
    }

    close(fd);
    std::cout << "[writer] done, FIFO write-end closed\n";
}

// ── Reader: receives messages until EOF ───────────────────────────────────

static void run_reader() {
    std::cout << "[reader] opening FIFO (blocks until writer opens)...\n";

    // O_RDONLY blocks here until a writer opens the other end
    int fd = open(FIFO_PATH, O_RDONLY);
    if (fd < 0) {
        perror("open(FIFO_PATH, O_RDONLY)");
        _exit(1);
    }

    std::cout << "[reader] connected\n";
    char buf[256];
    ssize_t bytes;
    while ((bytes = read(fd, buf, sizeof(buf) - 1)) > 0) {
        buf[bytes] = '\0';
        std::cout << "[reader] received: " << buf;
    }
    // bytes == 0 → writer closed (EOF)
    // bytes < 0  → error

    close(fd);
    std::cout << "[reader] EOF, done\n";
}

// ── Self-contained demo: fork writer and reader ───────────────────────────

static void run_demo() {
    std::cout << "=== Named pipe (FIFO) demo ===\n\n";

    // Create the FIFO (no-op if it already exists)
    if (mkfifo(FIFO_PATH, 0666) < 0 && errno != EEXIST) {
        perror("mkfifo");
        return;
    }
    std::cout << "Created FIFO at " << FIFO_PATH << "\n\n";

    // Fork writer first; it will block on open() until reader appears
    pid_t writer_pid = fork();
    if (writer_pid < 0) { perror("fork"); return; }
    if (writer_pid == 0) {
        run_writer();
        _exit(0);
    }

    // Fork reader; once both ends open, data flows
    pid_t reader_pid = fork();
    if (reader_pid < 0) { perror("fork"); return; }
    if (reader_pid == 0) {
        run_reader();
        _exit(0);
    }

    waitpid(writer_pid, nullptr, 0);
    waitpid(reader_pid, nullptr, 0);

    // Remove the FIFO from the filesystem
    unlink(FIFO_PATH);
    std::cout << "\nFIFO removed. Demo complete.\n";
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        std::string mode = argv[1];
        if (mode == "writer") {
            // Create FIFO if not present
            if (mkfifo(FIFO_PATH, 0666) < 0 && errno != EEXIST) {
                perror("mkfifo");
                return 1;
            }
            run_writer();
        } else if (mode == "reader") {
            run_reader();
            unlink(FIFO_PATH);
        } else {
            std::cerr << "Usage: " << argv[0] << " [writer|reader]\n";
            return 1;
        }
    } else {
        run_demo();
    }
    return 0;
}
