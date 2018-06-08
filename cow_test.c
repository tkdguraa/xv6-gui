#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(int argc, char *argv[])
{
    int* x = malloc(sizeof(int) * 1000);
    for (int i = 0; i <1000; i++)
        x[i] = i;
    printf(1, "x: %d\n", x[99]);
    exit();
}