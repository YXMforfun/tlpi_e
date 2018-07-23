#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"


int
main(int argc, char const *argv[]) {
    int fd1, fd2, fd3;
    char *file = "test_5_6.txt";
    fd1 = open(file, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
    fd2 = dup(fd1);
    fd3 = open(file, O_RDWR);
    // Hello,
    write(fd1, "Hello,", 6);
    // Hello,world
    write(fd2, "world", 6);
    // 文件偏移量移到文件首
    lseek(fd2, 0, SEEK_SET);
    // HELLO,world
    write(fd1, "HELLO,", 6);
    // Giddayworld
    write(fd3, "Gidday", 6);

    exit(EXIT_SUCCESS);
}
