/* zombie.c — Demonstrates a zombie process */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) { perror("fork"); exit(1); }

    if (pid == 0) {
        /* Child exits immediately — becomes zombie */
        printf("Child (PID %d): I'm done. Exiting now.\n", getpid());
        exit(0);
    }

    /* Parent does NOT call wait() */
    printf("Parent (PID %d): Child PID is %d. I will NOT call wait().\n", getpid(), pid);
    printf("Parent: Sleeping 60 seconds... check: ps aux | grep Z\n");
    sleep(60);
    printf("Parent: Exiting now. Zombie will be cleaned up by init.\n");
    return 0;
}
