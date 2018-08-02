#include <unistd.h>
#include <sys/fsuid.h>
#include <limits.h>
#include "ugid_functions.h"
#include "tlpi_hdr.h"


#define SG_SIZE (NGROUPS_MAX + 1)


int
main(int argc, char const *argv[]) {
    uid_t ruid, euid, suid, fsuid;
    gid_t rgid, egid, sgid, fsgid;
    gid_t supp_groups[SG_SIZE];
    int num_groups, j;
    char *username, group_name;



    return 0;
}
