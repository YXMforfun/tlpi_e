/*
./a.out 20 10
Initial program break:                 0x55915eb64000
Allocationg 20 * 10 bytes
num-alloc 0 malloc program break is: 0x55915eb85000
num-alloc 1 malloc program break is: 0x55915eb85000
num-alloc 2 malloc program break is: 0x55915eb85000
num-alloc 3 malloc program break is: 0x55915eb85000
num-alloc 4 malloc program break is: 0x55915eb85000
num-alloc 5 malloc program break is: 0x55915eb85000
num-alloc 6 malloc program break is: 0x55915eb85000
num-alloc 7 malloc program break is: 0x55915eb85000
num-alloc 8 malloc program break is: 0x55915eb85000
num-alloc 9 malloc program break is: 0x55915eb85000
num-alloc 10 malloc program break is: 0x55915eb85000
num-alloc 11 malloc program break is: 0x55915eb85000
num-alloc 12 malloc program break is: 0x55915eb85000
num-alloc 13 malloc program break is: 0x55915eb85000
num-alloc 14 malloc program break is: 0x55915eb85000
num-alloc 15 malloc program break is: 0x55915eb85000
num-alloc 16 malloc program break is: 0x55915eb85000
num-alloc 17 malloc program break is: 0x55915eb85000
num-alloc 18 malloc program break is: 0x55915eb85000
num-alloc 19 malloc program break is: 0x55915eb85000
Program break is now:                  0x55915eb85000
Freeing blocks from 1 to 20 in steps of 1
After free(), program break is: 0x55915eb85000

*/

#include "tlpi_hdr.h"

#define MAX_ALLOCS 1000000


int
main(int argc, char const *argv[]) {
    char *ptr[MAX_ALLOCS];
    int freeStep, freeMin, freeMax, blockSize, numAllocs, i;

    if (argc < 3 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s num-allocs block-size [step [min [max]]] \n", argv[0]);
    }

    numAllocs = getInt(argv[1], GN_GT_0, "num-allocs");
    if (numAllocs > MAX_ALLOCS) {
        cmdLineErr("num-allocs > %d\n", MAX_ALLOCS);
    }

    blockSize = getInt(argv[2], GN_GT_0 | GN_ANY_BASE, "block-size");

    freeStep = (argc > 3) ? getInt(argv[3], GN_GT_0, "step") : 1;
    freeMin = (argc > 4) ? getInt(argv[4], GN_GT_0, "min") : 1;
    freeMax = (argc > 5) ? getInt(argv[5], GN_GT_0, "max") : numAllocs;

    if (freeMax > numAllocs) {
        cmdLineErr("free-max > num-allocs\n");
    }

    printf("Initial program break:                 %10p\n", sbrk(0));

    printf("Allocationg %d * %d bytes\n", numAllocs, blockSize);
    for (i = 0; i < numAllocs; i++) {
        ptr[i] = malloc(blockSize);
        if (ptr[i] == NULL) {
            errExit("malloc falied");
        }
        printf("num-alloc %d malloc program break is: %10p\n", i, sbrk(0));
    }

    printf("Program break is now:                  %10p\n", sbrk(0));

    printf("Freeing blocks from %d to %d in steps of %d\n", freeMin, freeMax, freeStep);

    for (i = freeMin - 1; i < freeMax; i += freeStep) {
        free(ptr[i]);
    }

    printf("After free(), program break is: %10p\n", sbrk(0));

    exit(EXIT_SUCCESS);
}
