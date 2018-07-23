#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include "tlpi_hdr.h"


#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif


int
main(int argc, char const *argv[]) {
    size_t len;
    off_t offset;
    int inputFd, outputFd, openFlags;
    mode_t filePerms;
    ssize_t numRead;
    char buf[BUF_SIZE];

    if (argc < 3 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s need two files name", argv[0]);
    }

    inputFd = open(argv[1], O_RDONLY);
    if (inputFd == -1) {
        errExit("opening file %s failed", argv[1]);
    }

    openFlags = O_CREAT | O_TRUNC | O_WRONLY;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH;

    outputFd = open(argv[2], openFlags, filePerms);
    if (outputFd == -1) {
        errExit("opening file %s failed", argv[2]);
    }

    while ((numRead = read(inputFd, buf, BUF_SIZE)) > 0) {
        for (int i = 0; i < numRead; i++) {
            if (buf[i] == '\0') {
                if (lseek(outputFd, 1, SEEK_CUR) == -1) {
                    errExit("lseek failed");
                }
            } else {
                if (write(outputFd, &buf[i], 1) == -1) {
                    errExit("write failed");
                }
            }
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


    return 0;
}
