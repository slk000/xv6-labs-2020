#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"


#define ARGLEN 32 // will fail if too large
#define NULL 0

void
xargs(int largc, char largv[MAXARG][ARGLEN], int rargc, char *rargv[])
{
    
    // char argv[MAXARG][ARGLEN]; // in order to make argv[last]=NULL, I use an array of pointers below instead of 2d array. and re-use the existing spaces
    char *argv[MAXARG]; // final args for exec
    int argc = 1; // first arg is the executable filename
    argv[0] = rargv[1];

    for (int i = 2; i < rargc; i++) { // copy args of xargs to the front of final args
        // strcpy(argv[argc], rargv[i]);
        argv[argc] = rargv[i]; // copy pointers, re-use space
        argc++;
    }

    
    // printf("argc:%d args0:%s\n", argc, argv[0]);
    // printf("largc:%d largs0:%s\n", largc, largv[0]);
    for(int i = 0; i < largc; i++) { // for each line of left stdout
        char *start = &largv[i][0];      // split by space
        char *end = start;
        int cnt = argc;
        while(1) {
            char t = *end;
            if (t == ' ') {
                *end = '\0'; // combine the situations: *end==\0 and *end==' '
            }
            if (*end == '\0') {
                int nbytes = end - start + 1;
                // memcpy(argv[cnt], start, nbytes);
                argv[cnt] = start; // copy to re-use rather than memcpy
                if (nbytes > 1) cnt++; // nbytes means an empty arg copyied
                start = end+1;
            }
            if (t == '\0') { // original char==\0 means split finish
                break;
            }
            end++;
        }

        argv[cnt] = NULL;

        // printf("ready to run %s with args:\n", rargv[1]);
        // for (int j = 0; j < cnt; j++) {
        //     printf("%s ", argv[j]);
        // }
        // printf("\n");
        // printf("%p\n -------\n", argv[cnt]); // should == 0

        int pid = fork();
        if (pid > 0) {
            wait(0);
        }
        else if (pid == 0) {
            exec(rargv[1], argv);
            fprintf(2, "exec failed\n");
        }
        else {
            fprintf(2, "fork failed\n");
        }


    }
    
}

int
main(int argc, char *argv[])
{
    int largc = 0;
    char largv[MAXARG][ARGLEN]; // left argv, store by lines

    int cnt = 0;
    char *p = &largv[0][0]; // current char in one arg
    char ch;

    // split by \n
    while(cnt = read(0, p, 1)) {
        // printf("read: %c\n", *p);
        if (*p == '\n') {
            *p = 0;
            largc++;
            p = &largv[largc][0];
            continue;
        }
        p++;
    }
    xargs(largc, largv, argc, argv);

    exit(0);
}