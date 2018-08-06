#define _GNU_SOURCE
#include <limits.h>
#include <sys/stat.h>
#include <ctype.h>
#include <dirent.h>
#include <stdbool.h>
#include <pwd.h>
#include <grp.h>
#include "tlpi_hdr.h"

#define MAX_LINE 1000


struct callback_data {
    uid_t uid;
    char *path;
};


typedef struct callback_data cdata;
typedef int (*file_call_back)(void *user_data);
int strAfterOffset(char *base, char *cpy, int offset, int len);
cdata * initCallBackData(uid_t uid, char *path);


int
findUidInProcDir(uid_t uid, file_call_back uidStatusProc) {
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

        snprintf(path, PATH_MAX, "/proc/%s/status", dp->d_name);

        cdata *data = initCallBackData(uid, path);
        if (uidStatusProc((void *)data) == -1) {
            continue;
        }
    }

    return 0;
}


int
uidStatusProc(void *user_data) {
    if (user_data == NULL) {
        return -1;
    }
    cdata *data = (cdata *)user_data;
    uid_t uid = data->uid;
    char *path = data->path;


    bool match_uid = false;
    char line[MAX_LINE], name[MAX_LINE], pid[MAX_LINE];
    char *p;
    uid_t puid;

    FILE *fp;
    fp = fopen(path, "r");
    if (fp == NULL) {
        return -1;
    }

    while (fgets(line, MAX_LINE, fp) != NULL) {
        if (strncmp(line, "Uid:", 4) == 0) {
            puid = strtol(line + 4, NULL, 10);
            if (puid == uid) {
                match_uid = true;
            }
        }

        if (strncmp(line, "Name:", 5) == 0) {
            if (strAfterOffset(line, name, 5, MAX_LINE) == -1) {
                errExit("uidStatusProc strAfterOffset failed Name");
            }
        }

        if (strncmp(line, "Pid:", 4) == 0) {
            if (strAfterOffset(line, pid, 4, MAX_LINE) == -1) {
                errExit("uidStatusProc strAfterOffset failed Pid");
            }
        }
    }

    if (match_uid) {
        int proc_id = strtol(pid, NULL, 10);
        printf("Pid: Process %d %s", proc_id, name);
    }

    return 0;
}


int
strAfterOffset(char *base, char *cpy, int offset, int len) {
    if (*base == '\0' || base == NULL) {
        return -1;     
    }
    char *p;
    p = base + offset;
    while ((*p != '\0') && isspace((unsigned char) *p)) {
        p++;
    }

    strncpy(cpy, p, len - 1);
    cpy[len - 1] = '\0';

    return 0;
}


cdata * 
initCallBackData(uid_t uid, char *path) {
    cdata *data;
    data = malloc(sizeof(cdata));
    if (data == NULL) {
        return NULL;
    }

    data->uid = uid;
    data->path = path;

    return data;
}



uid_t
userIdFromName(char *username) {
    struct passwd *pwd;
    uid_t u;
    char *endptr;

    if (username == NULL || *username == '\0') {
        return -1;
    }

    u = strtol(username, &endptr, 10);
    if (*endptr == '\0') {
        return u;
    }           

    pwd = getpwnam(username);
    if (pwd == NULL) {
        return -1;
    }

    return pwd->pw_uid;
}


int
main(int argc, char *argv[]) {
    uid_t uid;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s username\n", argv[0]);
    }

    uid = userIdFromName(argv[1]);
    if (uid == -1) {
        cmdLineErr("Bad username: %s\n", argv[1]);
    }

    findUidInProcDir(uid, uidStatusProc);
    return 0;
}
