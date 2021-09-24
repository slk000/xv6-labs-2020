#include "kernel/types.h" // for uint
#include "user/user.h" // for print & atoi

int
main(int argc, char *argv[])
{
    if (argc <= 1) {
        fprintf(2, "Usage: ");
        exit(1);
    }

    uint ticks = atoi(argv[1]);
    sleep(ticks);
    exit(0);
}