#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
    char buff = 'P';
    int p2c[2], c2p[2];
    pipe(p2c);
    pipe(c2p);

    int pid = fork();
    if (pid == 0) {
        int child = getpid();

        read(p2c[0], &buff, 1);
        close(p2c[0]);
        printf("%d: received ping\n", child);

        write(c2p[1], &buff, 1);
        close(c2p[1]);
        exit(0);
    }
    else {
        int parent = getpid();

        write(p2c[1], &buff, 1);
        close(p2c[1]);

        read(c2p[0], &buff, 1);
        close(c2p[0]);
        printf("%d: received pong\n", parent);
        exit(0);

    }
    close(p2c[0]);
    close(p2c[1]);
    close(c2p[0]);
    close(c2p[1]);

    exit(0);
}