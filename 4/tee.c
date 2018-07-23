#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include "tlpi_hdr.h"

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif


int
main(int argc, char const *argv[]) {
    int inputFd, outputFd, openFlags;
    mode_t filePerms;
    ssize_t numRead;
    char buf[BUF_SIZE];

    if (strcmp(argv[1], "--help") == 0) {
        usageErr("%s need file name to open\n -a means append data to file", argv[0]);
    }

    inputFd = STDIN_FILENO;

    if (strcmp(argv[1], "-a") == 0) {
        openFlags = O_APPEND| O_WRONLY;
        outputFd = open(argv[2], openFlags);
    } else {
        openFlags = O_CREAT | O_WRONLY | O_TRUNC;
        filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                    S_IROTH | S_IWOTH;
        outputFd = open(argv[1], openFlags, filePerms);
    }

    while ((numRead = read(inputFd, buf, BUF_SIZE)) > 0) {
        if (write(outputFd, buf, numRead) != numRead) {
            fatal("couldn't write whole buffer");
        }
    }

    if (numRead == -1) {
        errExit("read");
    }

    if (close(inputFd) == -1) {
        errExit("close input");
    }

    if (close(outputFd) == -1) {
        errExit("close output");
    }

    exit(EXIT_SUCCESS);
}
