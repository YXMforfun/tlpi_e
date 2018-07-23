#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include "tlpi_hdr.h"


int
dup_e(int oldFd) {
    int newFd;
    if (fcntl(oldFd, F_GETFD) < 0) {
        errno = EBADF;
        return -1;
    }

    newFd = fcntl(oldFd, F_DUPFD, 0);
    return newFd;
}

int
dup2_e(int oldFd, int newFd) {
    if (fcntl(oldFd, F_GETFD) < 0) {
        errno = EBADF;
        return -1;
    }

    if (oldFd == newFd) {
        return newFd;
    }

    int newFdStatus = fcntl(newFd, F_GETFD);
    if ((newFdStatus < 0) && errno != EBADF) {
        return -1;
    } else if (newFdStatus > 0){
        close(newFd);
    }

    return fcntl(oldFd, F_DUPFD, newFd);
}


int
main(int argc, char const *argv[]) {
    int fd1, fd2;
	char buf[30];
    memset(buf, '\0', sizeof(buf));
    
	fd1 = open(argv[1], O_RDONLY);
    if (fd1 == -1) {
        usageErr("open failed");
    }

	if (strcmp(argv[2], "dup1") == 0) {
		fd2 = dup_e(fd1);
        printf("fd1 %d fd2 %d\n", fd1, fd2);
	} else if (strcmp(argv[2], "dup2") == 0) {
        fd2 = dup2_e(fd1, 10);
        printf("fd1 %d fd2 %d\n", fd1, fd2);
    }

	read(fd1, &buf[0], 20);
	printf("%s", buf);
	read(fd2, &buf[0], 20);
	printf("%s\n", buf);
	close(fd1);
	close(fd2);

    return 0;
}
