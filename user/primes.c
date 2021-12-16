#include "kernel/types.h"
#include "user/user.h"

#ifdef DEBUG
#define log(n) {printf("%d: %d\n", getpid(), n);}
#else
#define log(n) {}
#endif

int
read_num(int *pipe) {
    // printf("read_num\n");
	int ret;
    log(11);
	if (0 == read(pipe[0], &ret, sizeof(ret))) {
		return -1;
	}
	
	return ret;
}

int
main(int argc, char *argv[])
{
	int left[2], right[2];
	if (-1 == pipe(left)) {
        printf("create left pipe failed\n");
        exit(-1);
    }
	if (-1 == pipe(right)) {
        printf("create right pipe failed\n");
        exit(-1);
    }
	
    // printf("initialize\n");
    for (int i = 2; i <= 35; i++) {
        write(left[1], &i, sizeof(i));
    }
    
start:;
    log(1);

    close(left[1]); // ensure the last child exits. otherwise deadlock
    // close(right[0]); // do not use

    int prime = read_num(left);
    if (prime == -1) {
        log(2);
        close(left[0]);
        close(left[1]);
        close(right[0]);
        close(right[1]);
        exit(0);
    }
	printf("prime %d\n", prime);
    
	int pid = fork();
    log(3);
	if (pid > 0) {
        // printf("create child: %d\n", pid);

        int num;
		while(-1 != (num = read_num(left))) {
            log(6);
			if (num % prime != 0) {
				write(right[1], &num, sizeof(num));
			}
		}
        log(7);
	}
	else if (pid == 0) {
        log(8);

        left[0] = right[0];
        left[1] = right[1];
        pipe(right);
		goto start;
	}

    log(9);
    close(left[0]);
    close(left[1]);
    close(right[0]);
    close(right[1]);
    wait(0);

    log(10);
	exit(0);
}

