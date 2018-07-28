#include <pwd.h>
#include <shadow.h>
#include <stdio.h>
#include "tlpi_hdr.h"

/* In Exercise 8-1, completely replace the text:

    8-1. When we execute the following code, we find that it displays the same number twice, even though the two users have different IDs in the password file. Why is this?

        printf("%ld %ld\n", (long) (getpwnam("avr")->pw_uid),
                            (long) (getpwnam("tsr")->pw_uid));

with the following text:

    8-1. When we execute the following code, which attempts to display the usernames for two different user IDs, we find that it displays the same username twice. Why is this?

        printf("%s %s\n", getpwuid(uid1)->pw_name,
                          getpwuid(uid2)->pw_name);

勘误.传入不同 UID 得到相同的名字.
    会得到第一个输入的 UID 的 usernames.
    因为 getpwnam() 返回的指针指向由静态分配而成的内存, 是不可重入的.
*/

int
main(int argc, char const *argv[]) {
    if (argc != 3 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s uid1 uid2\n", argv[0]);
        return -1;
    }
    printf("%s %s\n", getpwuid(atoi(argv[1]))->pw_name,
                      getpwuid(atoi(argv[2]))->pw_name);
    return 0;
}
