#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdbool.h>
#include "tlpi_hdr.h"

/*
    ./5_3.o abcd.log 1000000 x & ./5_3.o abcd.log 1000000 x
    ./5_3.o abc.log 1000000 & ./5_3.o abc.log 1000000

    abcd.log 1.5M wc abcd.log 1561136
    abc.log 2.0M wc abc.log 2000000

    第一个进程执行到 lseek() 和 write() 之间, 被执行相同代码的第二个
    进程所中断, 那么这两个进程会在写入数据前, 将文件偏移量设为相同位置,
    而当第一个进程再次获得调度时, 会覆盖第二个进程已写入的数据.
*/

int
main(int argc, char const *argv[]) {
    int outputFd, openFlags, strLen;
    mode_t filePerms;
    off_t offset;
    bool status;
    char inputChar = 'y';

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("5_3.o filename num-bytes [x]");
    }

    status = false;
    openFlags = O_APPEND | O_WRONLY | O_CREAT;
    strLen = atoi(argv[2]);
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH;

    if ((argc == 4) && (strcmp(argv[3], "x") == 0)) {
        openFlags = O_WRONLY | O_CREAT;
        status = true;
    }

    outputFd = open(argv[1], openFlags, filePerms);
    if (outputFd == -1) {
        errExit("open file %s falied", argv[1]);
    }

    for (int i = 0; i < strLen; i++) {
        if (status == true) {
            offset = lseek(outputFd, 0, SEEK_END);
            if (offset == -1) {
                errExit("lseek set failed");
            }
        }
        if (write(outputFd, &inputChar, 1) == -1) {
            errExit("write failed");
        }
    }

    exit(EXIT_SUCCESS);
}
