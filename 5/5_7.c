#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/uio.h>
#include <limits.h>
#include <errno.h>
#include <malloc.h>
#include <stdbool.h>
#include "tlpi_hdr.h"


ssize_t
writev_e(int fd, const struct iovec *iov, int iovcnt) {
    int i, bytes;
    struct iovec *vec;
    unsigned long long cnt = 0;
    ssize_t total_bytes = 0;

    /* 检查 入参 */
    if (iovcnt <= 0) {
        errno = EINVAL;
        return -1;
    }

    for (i = 0; i < iovcnt; i++) {
        vec = (struct iovec *)&iov[i];
        cnt += vec->iov_len;
        if (cnt > SSIZE_MAX) {
            errno = EINVAL;
            return -1;
        }
    }

    for (i = 0; i < iovcnt; i++) {
        vec = (struct iovec *)&iov[i];
        bytes = write(fd, vec->iov_base, vec->iov_len);
        if (bytes < 0) {
            return bytes;
        } else {
            total_bytes += bytes;
            if (bytes < vec->iov_len) {
                return total_bytes;
            }
        }
    }

    return total_bytes;
}


ssize_t
readv_e(int fd, const struct iovec *iov, int iovcnt) {
    int i, bytes;
    struct iovec *vec;
    unsigned long long cnt = 0;
    ssize_t total_bytes = 0;

    /* 检查 入参 */
    if (iovcnt <= 0) {
        errno = EINVAL;
        return -1;
    }

    for (i = 0; i < iovcnt; i++) {
        vec = (struct iovec *)&iov[i];
        cnt += vec->iov_len;
        if (cnt > SSIZE_MAX) {
            errno = EINVAL;
            return -1;
        }
    }

    for (i = 0; i < iovcnt; i++) {
        vec = (struct iovec *)&iov[i];
        bytes = read(fd, vec->iov_base, vec->iov_len);
        if (bytes < 0) {
            return bytes;
        } else {
            total_bytes += bytes;
            if (bytes < vec->iov_len) {
                return total_bytes;
            }
        }
    }

    return total_bytes;
}


int
main(int argc, const char *argv[]) {
    int vecNums = 50;
    int iovLen = 64;
    int fd, i;
    struct iovec read_iovecs[vecNums];
    struct iovec write_iovecs[vecNums];

    for (i = 0; i < vecNums; i++) {
        void *mem;

        mem = malloc(iovLen);
        if (mem == NULL) {
            errExit("malloc failed");
        }
        memset(mem, 0, iovLen);
        read_iovecs[i].iov_base = mem;
        read_iovecs[i].iov_len = iovLen;

        mem = malloc(iovLen);
        memset(mem, 0, iovLen);
        write_iovecs[i].iov_base = mem;
        write_iovecs[i].iov_len = iovLen;
    }

    fd = open(argv[1], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    ssize_t write_bytes = writev_e(fd, &write_iovecs[0], iovLen);
    if (write_bytes < (iovLen * vecNums)) {
        errExit("write failed");
    }
    close(fd);

    fd = open(argv[1], O_RDONLY);
    ssize_t read_bytes = readv_e(fd, &read_iovecs[0], iovLen);
    if (read_bytes < (iovLen * vecNums)) {
        errExit("read failed");
    }

    bool isEqual = true;
    for (i = 0; i < vecNums; i++) {
        if (memcmp((void *)write_iovecs[i].iov_base, (void *)read_iovecs[i].iov_base, iovLen) != 0) {
            isEqual = false;
            break;
        }
    }

    if (isEqual == true) {
        printf("readv writev success");
    } else {
        printf("readv writev failed");
    }

    return 0;
}
