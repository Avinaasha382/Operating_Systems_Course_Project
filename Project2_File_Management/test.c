#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int fd = open("example.txt", O_RDWR);
    if (fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    struct flock lock;
    lock.l_type = F_WRLCK;  // Request a write lock
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;         // Lock the whole file

    // Apply the lock
    if (fcntl(fd, F_SETLK, &lock) == -1) {
        perror("Error locking file");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("File locked. Press Enter to continue and launch gedit.\n");
    getchar();

    pid_t pid = fork();
    if (pid == -1) {
        perror("Error forking process");
        close(fd);
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process: Launch gedit
        char *argv[] = {"gedit", "example.txt", NULL};
        execv("/usr/bin/gedit", argv);
        perror("Error executing gedit");
        exit(EXIT_FAILURE);
    } else {
        // Parent process: Wait for child process to finish
        wait(NULL);

        // Unlock the file
        lock.l_type = F_UNLCK;
        if (fcntl(fd, F_SETLK, &lock) == -1) {
            perror("Error unlocking file");
        }

        close(fd);
        printf("File unlocked.\n");
    }

    return 0;
}
