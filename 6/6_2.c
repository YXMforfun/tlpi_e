/*
        setJump()
        setJump 0
        setJump 1
        [1]    13130 segmentation fault  ./a.out

==13755== Process terminating with default action of signal 11 (SIGSEGV)
==13755==  Bad permissions for mapped region at address 0x1FFEFFFEC8

*/
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>


static jmp_buf env;


int
setJump(int open) {
    printf("setJump()\n");
    if (open) {
        switch (setjmp(env)) {
            case 0:
                printf("setJump 0\n");
                break;

            case 1:
                printf("setJump 1\n");
                break;
        }
    }
    return 0;
}


int
main(int argc, const char *argv[]) {
    (void)argc;
    (void)argv;
    setJump(1);
    longjmp(env, 1);
    setJump(0);
    longjmp(env, 0);

    exit(EXIT_SUCCESS);
}
