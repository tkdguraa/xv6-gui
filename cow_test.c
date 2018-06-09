#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
    int pid, i;
    int SIZE = 4096 + 1; // two pages, will occur page fault 2 times. 
    char *space = (char*)malloc(SIZE);
    printf(1, "=====start cow test=====\n", space[0]);

    if ((pid = fork()) == 0) {
        printf(1, "---child process is created\n");
        printf(1, "---child process changed memory values\n");
        for (i = 0; i < SIZE; i++)
            // space[i] = i;

        exit();
    }
    else {
        printf(1, "---child process pid: %d\n",pid);
        wait();
    }

    // free(space);
    exit();
}