#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>


int
unsetenv(const char *name) {
    extern char **environ;
    char **ep, **sp;
    size_t len;

    if (name == NULL || name[0] == '\0' || strchr(name, '=') != NULL) {
        errno = EINVAL;
        return -1;
    }

    len = strlen(name);

    ep = environ;
    while (*ep != NULL) {
        if (strncmp(*ep, name, len) == 0 && (*ep)[len] == '=') {
            for (sp = ep; *sp != NULL; sp++) {
                *sp = *(sp + 1);
            }
        } else {
            ep++;
        }
    }

    return 0;
}


int
setenv(const char *name, const char *value, int overwrite) {
    char *es;

    if (name == NULL || name[0] == '\0' || strchr(name, '=') != NULL ||
            value == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (getenv(name) != NULL && overwrite == 0) {
        return 0;
    }

    unsetenv(name);

    es = malloc(strlen(name) + strlen(value) + 2);

    if (es == NULL) {
        return -1;
    }

    strcpy(es, name);
    strcat(es, "=");
    strcat(es, value);

    if (putenv(es) != 0) {
        return -1;
    } else {
        return 0;
    }
}


int
main(int argc, char const *argv[]) {
    if (putenv("abc=abc") != 0) {
        perror("putenv failed");
    }

    system("echo 'before unsetenv abc;'; printenv | grep abc");

    unsetenv("abc");

    system("echo 'after unsetenv abc;'; printenv | grep abc");

    setenv("abc", "abc", 0);

    system("echo 'after setenv abc;'; printenv | grep abc");

    setenv("abc", "def", 1);

    system("echo 'after setenv abc;'; printenv | grep abc");

    return 0;
}
