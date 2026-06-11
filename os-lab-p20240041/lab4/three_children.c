#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int i;
    pid_t pids[3];

    for (i = 0; i < 3; i++) {
        pids[i] = fork();
        if (pids[i] == 0) {
            /* Each child sleeps a different amount */
            printf("Child %d (PID %d): sleeping %d seconds\n", i+1, getpid(), (i+1)*60);
            sleep((i+1) * 10);
            printf("Child %d (PID %d): done\n", i+1, getpid());
            exit(0);
        }
    }

    printf("Parent (PID %d): spawned 3 children. Run: ps --forest\n", getpid());
    printf("PIDs: %d, %d, %d\n", pids[0], pids[1], pids[2]);

    /* Wait for all 3 children */
    for (i = 0; i < 3; i++) {
        wait(NULL);
        printf("Parent: one child finished.\n");
    }

    printf("Parent: all children done. Exiting.\n");
    return 0;
}
