/**
 * PLATFORM MECHANICS: POSIX signal handlers and explicit child reaping.
 *
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
 *   g++ -std=c++20 -o signals signals.cpp
 */

#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/wait.h>
#include <unistd.h>

// Block before fork so a signal cannot arrive before the child is ready.
static sigset_t block_signal(int signum) {
    sigset_t set, previous;
    sigemptyset(&set);
    sigaddset(&set, signum);
    if (sigprocmask(SIG_BLOCK, &set, &previous) < 0) {
        perror("sigprocmask");
        std::exit(1);
    }
    return previous;
}

static bool wait_for_child(pid_t pid, int& status) {
    while (waitpid(pid, &status, 0) < 0) {
        if (errno == EINTR) continue;
        perror("waitpid");
        return false;
    }
    return true;
}

static volatile sig_atomic_t signal_received = 0;

static void control_handler(int /*signum*/) {
    signal_received = 1;
}

// sigsuspend atomically unblocks the signal and waits. The flag is checked
// while it is blocked, closing the check-before-pause race.
static void demo_control_signal(int signum, const char* name) {
    printf("=== %s: cooperative child shutdown ===\n", name);
    signal_received = 0;
    sigset_t previous = block_signal(signum);
    sigset_t waiting = previous;
    sigdelset(&waiting, signum);
    struct sigaction sa{}, old_action{};
    sa.sa_handler = control_handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(signum, &sa, &old_action) < 0) {
        perror("sigaction");
        sigprocmask(SIG_SETMASK, &previous, nullptr);
        return;
    }
    fflush(stdout);
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
    } else if (pid == 0) {
        printf("  Child: waiting for %s (PID=%d)\n", name, getpid());
        fflush(stdout);
        while (!signal_received) sigsuspend(&waiting);
        printf("  Child: received %s, shutting down cleanly\n", name);
        fflush(stdout);
        _exit(0);
    } else {
        // Safe even if the child has not started: the signal stays pending.
        printf("  Parent: sending %s to child (PID=%d)\n", name, pid);
        if (kill(pid, signum) < 0) perror("kill");
        sigprocmask(SIG_SETMASK, &previous, nullptr);
        int status;
        if (wait_for_child(pid, status) && WIFEXITED(status))
            printf("  Parent: child exited, status=%d\n\n", WEXITSTATUS(status));
    }
    sigaction(signum, &old_action, nullptr);
    sigprocmask(SIG_SETMASK, &previous, nullptr);
}

static volatile sig_atomic_t children_reaped = 0;

static void sigchld_handler(int /*signum*/) {
    int saved_errno = errno;
    while (true) {
        pid_t pid = waitpid(-1, nullptr, WNOHANG);
        if (pid > 0) {
            children_reaped = children_reaped + 1;
        } else if (pid < 0 && errno == EINTR) {
            continue;
        } else {
            break;
        }
    }
    errno = saved_errno;
}

static void demo_sigchld() {
    printf("=== SIGCHLD: auto-reaping ===\n");
    children_reaped = 0;
    sigset_t previous = block_signal(SIGCHLD);
    sigset_t waiting = previous;
    sigdelset(&waiting, SIGCHLD);
    struct sigaction sa{}, old_action{};
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa, &old_action) < 0) {
        perror("sigaction");
        sigprocmask(SIG_SETMASK, &previous, nullptr);
        return;
    }

    constexpr int n_children = 4;
    int spawned = 0;
    for (int i = 0; i < n_children; ++i) {
        fflush(stdout);
        pid_t pid = fork();
        if (pid < 0) { perror("fork"); continue; }
        if (pid == 0) {
            sigprocmask(SIG_SETMASK, &previous, nullptr);
            printf("  Child %d exiting\n", i);
            fflush(stdout);
            _exit(i * 10);
        }
        ++spawned;
        printf("  Spawned child %d (PID=%d)\n", i, pid);
    }

    // A single delivery may reap multiple children. Count successful waits.
    while (children_reaped < spawned) sigsuspend(&waiting);
    printf("  Parent: all %d children reaped.\n\n", spawned);
    // Restore the handler so the next demo can wait for its own child.
    sigaction(SIGCHLD, &old_action, nullptr);
    sigprocmask(SIG_SETMASK, &previous, nullptr);
}

int main() {
    demo_control_signal(SIGTERM, "SIGTERM");
    demo_sigchld();
    demo_control_signal(SIGUSR1, "SIGUSR1");
    return 0;
}
