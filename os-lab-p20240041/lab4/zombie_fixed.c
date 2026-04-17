#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) { perror("fork"); exit(1); }

    if (pid == 0) {
        printf("Child (PID %d): Exiting now.\n", getpid());
        exit(0);
    }

    printf("Parent (PID %d): Child is %d. Sleeping 10 seconds before calling wait()...\n", getpid(), pid);
    sleep(10);   /* Zombie exists during this 10-second window */

    int status;
    wait(&status);   /* This cleans up the zombie */
    printf("Parent: Called wait(). Zombie is gone.\n");

    return 0;
}
