#define _GNU_SOURCE
#include <limits.h>
#include <ctype.h>
#include <dirent.h>
#include <stdbool.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#include "tlpi_hdr.h"

#define MAX_LINE 1000


struct callback_data {
    char *pid;
    char *path;
};


typedef struct callback_data cdata;
typedef int (*file_call_back)(void *user_data);
cdata * initCallBackData(char *dp_name, char *path);


int
iterFileInProcDir(file_call_back iterFdProc) {
    DIR *dir;
    struct dirent *dp;
    char path[PATH_MAX];

    dir = opendir("/proc");
    if (dir == NULL) {
        errExit("openProcDir opendir failed");
    }

    while(true) {
        // 用于分辨　errno　是不是到达目录最后 
        errno = 0;
        dp = readdir(dir);
        if (dp == NULL) {
            if (errno != 0) {
                errExit("openProcDir readdir failed");
            } else {
                break;
            }
        }

        if (dp->d_type != DT_DIR || !isdigit((unsigned char)dp->d_name[0])) {
            continue;
        }

        snprintf(path, PATH_MAX, "/proc/%s/fd", dp->d_name);

        cdata *data = initCallBackData(dp->d_name, path);
        if (iterFdProc((void *)data) == -1) {
            continue;
        }
    }

    return 0;
}


int
iterFdProc(void *user_data) {
    if (user_data == NULL) {
        return -1;
    }
    cdata *data = (cdata *)user_data;
    char *path = data->path;
    char *pid = data->pid;

    DIR *dir;
    struct dirent *dp;
    char *link_path, *link;
    int link_len;

    dir = opendir(path);
    if (dir == NULL) {
        errExit("iterFdProc opendir failed");
    }

    while(true) {
        // 用于分辨　errno　是不是到达目录最后 
        errno = 0;
        dp = readdir(dir);
        if (dp == NULL) {
            if (errno != 0) {
                errExit("iterFdProc readdir failed");
            } else {
                break;
            }
        }

        if (dp->d_type != DT_LNK) {
            continue;
        }

        strcpy(link_path, path);
        strcat(link_path, "/");
        strcat(link_path, dp->d_name);

        link_len = readlink(link_path, link, 1024);
        if (link_len < 0 || link_len > 1024 - 1) {
            break;
        }

        link[link_len] = '\0';

        printf("pid: %s link: %s link_len: %d\n", pid, link, link_len);
    }

    
    return 0;
}


cdata * 
initCallBackData(char *pid, char *path) {
    cdata *data;
    data = malloc(sizeof(cdata));
    if (data == NULL) {
        return NULL;
    }

    data->pid = pid;
    data->path = path;

    return data;
}


int
main(int argc, char *argv[]) {
    iterFileInProcDir(iterFdProc);
    return 0;
}
