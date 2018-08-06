#define _GNU_SOURCE
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include <dirent.h>
#include <stdbool.h>
#include <pwd.h>
#include <grp.h>
#include "tlpi_hdr.h"

#define MAX_LINE 1000


struct callback_data {
    char *path;
};

struct proc_tree_node {
    char name[MAX_LINE];
    pid_t pid;
    pid_t ppid;
    int children_size;
    struct proc_tree_node *parent;
    struct proc_tree_node *children[1024];
    struct proc_tree_node *next;
};


typedef struct callback_data cdata;
typedef struct proc_tree_node pnode;
typedef int (*file_call_back)(void *user_data);
int strAfterOffset(char *base, char *cpy, int offset, int len);
cdata * initCallBackData(char *path);
pnode * initProcTreeNode(char *name, pid_t pid, pid_t ppid);
pnode * appendProcTreeNode(pnode *head, pnode *node);


static pnode *plist_head = NULL;


int
iterFileInProcDir(file_call_back pidProc) {
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

        cdata *data = initCallBackData(path);
        if (pidProc((void *)data) == -1) {
            continue;
        }
    }

    return 0;
}


int
pidProc(void *user_data) {
    if (user_data == NULL) {
        return -1;
    }
    cdata *data = (cdata *)user_data;
    char *path = data->path;

    char line[MAX_LINE], name[MAX_LINE], spid[MAX_LINE], sppid[MAX_LINE];
    char *p;
    pid_t pid, ppid;
    pnode *n;

    FILE *fp;
    fp = fopen(path, "r");
    if (fp == NULL) {
        return -1;
    }

    while (fgets(line, MAX_LINE, fp) != NULL) {
        if (strncmp(line, "Name:", 5) == 0) {
            if (strAfterOffset(line, name, 5, MAX_LINE) == -1) {
                errExit("uidStatusProc strAfterOffset failed Name");
            }
        }

        if (strncmp(line, "Pid:", 4) == 0) {
            if (strAfterOffset(line, spid, 4, MAX_LINE) == -1) {
                errExit("uidStatusProc strAfterOffset failed Pid");
            }
            pid = strtol(spid, NULL, 10);

        }

        if (strncmp(line, "PPid:", 5) == 0) {
            if (strAfterOffset(line, sppid, 5, MAX_LINE) == -1) {
                errExit("uidStatusProc strAfterOffset failed PPid");
            }
            ppid = strtol(sppid, NULL, 10);
        }

    }
    fclose(fp);

    n = initProcTreeNode(name, pid, ppid);
    if (n != NULL) {
        plist_head = appendProcTreeNode(plist_head, n);
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
initCallBackData(char *path) {
    cdata *data;
    data = malloc(sizeof(cdata));
    if (data == NULL) {
        return NULL;
    }

    data->path = path;

    return data;
}


pnode *
initProcTreeNode(char *name, pid_t pid, pid_t ppid) {
    pnode *node;
    node = malloc(sizeof(pnode));
    if (node == NULL) {
        return NULL;
    }

    strcpy(node->name, name);
    node->pid = pid;
    node->ppid = ppid;
    node->children_size = 0;
    node->children[0] = NULL;
    node->parent = NULL;
    node->next = NULL;

    return node;
}


pnode *
appendProcTreeNode(pnode *head, pnode *node) {
    pnode *h;
    node->next = head;
    h = node;
    return h;
}


pnode *
findProcTreeNode(pnode *head, pid_t target_pid) {
    if (head == NULL) {
        return NULL;
    }

    pid_t cur_pid;
    pnode *curr;

    curr = head;
    while (curr != NULL) {
        cur_pid = curr->pid;
        if (cur_pid == target_pid) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}


void
matchParentChildNode(pnode *parent, pnode *child) {
    child->parent = parent;
    parent->children[parent->children_size] = child;
    parent->children_size++;
    parent->children[parent->children_size] = NULL;
}


pnode *
listToTree(pnode *head) {
    if (head == NULL) {
        return NULL;
    }

    pnode *curr, *parent;

    curr = head;
    while (curr != NULL) {
        parent = findProcTreeNode(head, curr->ppid);
        if (parent == NULL) {
            curr = curr->next;
            continue;
        }

        matchParentChildNode(parent, curr);
        curr = curr->next;
    }

    return head;
}


void
logProcList(pnode *head) {
    pid_t parent_pid;
    pnode *curr = head;
    int i = 0;
    while (curr != NULL) {
        if (curr->parent == NULL) {
            parent_pid = 0;
        } else {
            parent_pid = curr->parent->pid;
        }
        printf("index: %d pid: %d ppid: %d parent_id: %d name: %s", i, curr->pid, curr->ppid, parent_pid, curr->name);
        curr = curr->next;
        i += 1;
    }
}


int
main(int argc, char *argv[]) {
    iterFileInProcDir(pidProc);
    plist_head = listToTree(plist_head);
    logProcList(plist_head);
    return 0;
}
