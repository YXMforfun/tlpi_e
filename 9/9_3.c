/*
    需要 sudo 运行
    否则会 报错
    [1]    10003 segmentation fault  ./a.out
➜   ✗ valgrind ./a.out 
    ==10046== Memcheck, a memory error detector
    ==10046== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
    ==10046== Using Valgrind-3.13.0 and LibVEX; rerun with -h for copyright info
    ==10046== Command: ./a.out
    ==10046==
    ==10046== Use of uninitialised value of size 8
    ==10046==    at 0x4C33DA3: strcmp (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
    ==10046==    by 0x10A4E7: e_initgroups (in /home/forfun/tlpi/9/a.out)
    ==10046==    by 0x10A5F4: test (in /home/forfun/tlpi/9/a.out)
    ==10046==    by 0x10A64A: main (in /home/forfun/tlpi/9/a.out)
    ==10046==
    ERROR [EPERM Operation not permitted] e_initgroups failed

*/


#define _GNU_SOURCE
#include <unistd.h>
#include <sys/fsuid.h>
#include <limits.h>
#include <pwd.h>
#include <grp.h>
#include <ctype.h>
#include "tlpi_hdr.h"


#define SG_SIZE (NGROUPS_MAX + 1)


int
e_initgroups(const char *user, gid_t group) {
    struct group *grp;
    gid_t supp_groups[SG_SIZE];
    gid_t r_groups[SG_SIZE];
    int num_groups, i, j, groups_index;
    char *username, group_name;

    num_groups = getgroups(SG_SIZE, supp_groups);
    if (num_groups == -1) {
        errExit("getgroups failed");
    }

    groups_index = 0;
    r_groups[groups_index++] = group;

    for (i = 0; i < num_groups; i++) {
        grp = getgrgid(supp_groups[i]);
        j = 0;
        while((username = grp->gr_mem[j++]) != NULL) {
            if (strcmp(username, user) == 0) {
                r_groups[groups_index++] = grp->gr_gid;
            }
        }
    }

    return setgroups(groups_index, r_groups);
}


int
test() {
    const char *username;
    uid_t ruid, euid, suid;
    struct passwd *pwd;
    if (getresuid(&ruid, &euid, &suid) == -1) {
        errExit("getresuid failed");
    }

    pwd = getpwuid(ruid);

    if (e_initgroups(username, pwd->pw_gid) == -1) {
        errExit("e_initgroups failed");
    } else {
        printf("success\n");
    }

    return 0;
}


int
main(int argc, char const *argv[]) {
    test();

    return 0;
}
