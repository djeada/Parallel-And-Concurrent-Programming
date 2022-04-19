#include <iostream>
#include <unistd.h>

void spawnNumbers(int fd[2]) {
    int i = 0;
    while (i < 10) {
        write(fd[1], &i, sizeof(i));
        i++;
    }
}

int main() {
    int fd[2];
    pipe(fd);
    pid_t pid = fork();
    if (pid == 0) {
        spawnNumbers(fd);
    } else {
        int i = 0;
        while (i < 10) {
            read(fd[0], &i, sizeof(i));
            std::cout << i << std::endl;
            i++;
        }
    }
    return 0;
}
