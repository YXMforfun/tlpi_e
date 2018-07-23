#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include "tlpi_hdr.h"

/*结果是 在最后添加了数据
  将打开文件状态设为 O_APPEND 的时候, 此时 file pointer 强制设为文件末尾处
*/

int
main(int argc, char const *argv[]) {
    int outputFd;
    char *buf;
    off_t offset;

    if (argc < 3 || strcmp(argv[1], "--help") == 0) {
        usageErr("5_2.o filename data");
    }

    outputFd = open(argv[1], O_APPEND | O_WRONLY);
    if (outputFd == -1) {
        errExit("open file %s falied", argv[1]);
    }
    offset = lseek(outputFd, 0, SEEK_SET);
    if (offset == -1) {
        errExit("lseek set failed");
    }

    if (write(outputFd, argv[2], strlen(argv[2])) == -1) {
        errExit("write failed");
    }

    exit(EXIT_SUCCESS);
}
