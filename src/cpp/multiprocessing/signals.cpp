/**
 * POSIX Signals — sigaction, kill, SIGCHLD, SIGTERM
 *
 * Signals are asynchronous notifications sent to a process.
 * They are a lightweight IPC mechanism used for control events.
 *
 * Key rules for safe signal handlers:
 *   - Only call async-signal-safe functions (see man 7 signal-safety).
 *   - Do NOT use: printf, cout, malloc, new, mutexes, or C++ exceptions.
 *   - Use volatile sig_atomic_t flags and do real work in the main loop.
 *   - Use sigaction(), not signal() (signal() has undefined semantics).
 *
 * This file covers three common patterns:
 *   1. SIGTERM graceful shutdown (parent sends, child handles)
 *   2. SIGCHLD auto-reaping of zombie processes
 *   3. kill() to send arbitrary signals between processes
 *
 * Compile:
 *   g++ -std=c++17 -o signals signals.cpp
 */

#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <sys/wait.h>
#include <unistd.h>

// ── Demo 1: SIGTERM graceful shutdown ─────────────────────────────────────
// The handler only sets a flag; the main loop checks the flag.

static volatile sig_atomic_t shutdown_requested = 0;

static void sigterm_handler(int /*signum*/) {
    shutdown_requested = 1;  // async-signal-safe: just set a flag
}

static void demo_sigterm() {
    write(STDOUT_FILENO, "=== Demo 1: SIGTERM graceful shutdown ===\n", 42);

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return; }

    if (pid == 0) {
        // Child: install SIGTERM handler and run a loop
        struct sigaction sa{};
        sa.sa_handler = sigterm_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sigaction(SIGTERM, &sa, nullptr);

        write(STDOUT_FILENO, "  Child: running (PID=", 22);
        // print PID safely via write
        char buf[16];
        int n = snprintf(buf, sizeof(buf), "%d)\n", getpid());
        write(STDOUT_FILENO, buf, n);

        while (!shutdown_requested) {
            sleep(1);  // interruptible by signal
        }
        write(STDOUT_FILENO, "  Child: received SIGTERM, shutting down cleanly\n", 49);
        _exit(0);
    }

    // Parent: let child start, then send SIGTERM
    sleep(1);
    printf("  Parent: sending SIGTERM to child (PID=%d)\n", pid);
    kill(pid, SIGTERM);

    int status;
    waitpid(pid, &status, 0);
    printf("  Parent: child exited, status=%d\n\n", WEXITSTATUS(status));
}

// ── Demo 2: SIGCHLD auto-reaping ──────────────────────────────────────────
// SIGCHLD fires when a child changes state (exits, stops).
// The handler calls waitpid(-1, WNOHANG) in a loop to reap all finished
// children without blocking.  errno must be saved/restored.

static void sigchld_handler(int /*signum*/) {
    int saved_errno = errno;
    int status;
    while (waitpid(-1, &status, WNOHANG) > 0) {
        // reap the child; real work (logging) done outside handler
    }
    errno = saved_errno;
}

static volatile sig_atomic_t children_reaped = 0;

static void demo_sigchld() {
    printf("=== Demo 2: SIGCHLD auto-reaping ===\n");

    struct sigaction sa{};
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;  // restart interrupted syscalls
    sigaction(SIGCHLD, &sa, nullptr);

    constexpr int n_children = 4;
    for (int i = 0; i < n_children; ++i) {
        pid_t pid = fork();
        if (pid < 0) { perror("fork"); continue; }
        if (pid == 0) {
            sleep(i + 1);  // staggered exits
            printf("  Child %d exiting\n", i);
            _exit(i * 10);
        }
        printf("  Spawned child %d (PID=%d)\n", i, pid);
    }

    // Main loop: SIGCHLD interrupts sleep() to reap children promptly
    printf("  Parent: waiting for all children via SIGCHLD...\n");
    int remaining = n_children;
    while (remaining > 0) {
        pause();  // sleep until signal arrives
        // After waking, check how many children we've reaped
        // (a real app would track counts in the handler or use waitpid here)
        --remaining;  // simplified: one signal per exit for this example
        printf("  Parent: woke up, children remaining ~%d\n", remaining);
    }
    printf("  Parent: all children reaped.\n\n");
}

// ── Demo 3: kill() — sending signals between processes ────────────────────

static volatile sig_atomic_t usr1_received = 0;

static void sigusr1_handler(int /*signum*/) {
    usr1_received = 1;
}

static void demo_kill_signal() {
    printf("=== Demo 3: kill() — sending SIGUSR1 ===\n");

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return; }

    if (pid == 0) {
        struct sigaction sa{};
        sa.sa_handler = sigusr1_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sigaction(SIGUSR1, &sa, nullptr);

        printf("  Child: waiting for SIGUSR1...\n");
        while (!usr1_received) {
            pause();
        }
        printf("  Child: SIGUSR1 received, doing work and exiting\n");
        _exit(0);
    }

    sleep(1);
    printf("  Parent: sending SIGUSR1 to child (PID=%d)\n", pid);
    kill(pid, SIGUSR1);

    waitpid(pid, nullptr, 0);
    printf("  Parent: done\n\n");
}

int main() {
    demo_sigterm();
    demo_sigchld();
    demo_kill_signal();
    return 0;
}
