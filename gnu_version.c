#include <stdio.h>
#include <gnu/libc-version.h>
#include <string.h>

int
main() {
    const char* version;
    version = gnu_get_libc_version();
    printf("%s\n", version);
}
