#include <stdio.h>
#include <pwd.h>
#include <string.h>


struct passwd*
getPwName(const char *name) {
    struct passwd *pwd;
    while ((pwd = getpwent()) != NULL) {
        if (strcmp(pwd->pw_name, name) == 0) {
            return pwd;
        }
    }
    endpwent();
    return NULL;
}


void
logPw(struct passwd* pwd) {
    if (pwd == NULL) {
        printf("logPw failed\n");
        return;
    }
    printf("logPw\n");
    printf("pw_name = %s\n", pwd->pw_name);
    printf("pw_uid = %d\n", pwd->pw_uid);
    printf("pw_gid = %d\n", pwd->pw_gid);
    printf("pw_dir = %s\n", pwd->pw_dir);
    printf("pw_shell = %s\n", pwd->pw_shell);
    return;
}


int
main(int argc, char const *argv[]) {
    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        printf("getPwname name\n");
        return -1;
    }
    logPw(getPwName(argv[1]));
    return 0;
}
