#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>


struct memNodeStruct {
    size_t memLen;
    struct memNodeStruct *pre;
    struct memNodeStruct *next;
} memNodeStruct;

typedef struct memNodeStruct memNode;

static memNode *memNodeList = NULL;


/*
    创建新的空闲内存链表表头, 在 free 的时候触发, 替换空闲内存链表表头
    以传入的指针为起始点, 往内存地址减小的方向移动(相当于回收堆里的内存)
*/
void *
ptr_mem_head(void *ptr) {
    void *p;
    p = (void *)((unsigned long)ptr - sizeof(memNode));
    return p;
}


/*
    在 malloc 的时候触发, 计算提供内存块后的内存地址
    以传入的指针为起始点, 往内存地址增大的方向移动(相当于分配堆里的内存)
*/
void *
ptr_mem_node(void *ptr) {
    void *p;
    p = (void *)((unsigned long)ptr + sizeof(memNode));
    return p;
}


void
log_mem_list(memNode *header) {
    memNode *curr = header;
    int i = 0;
    while(curr != NULL) {
        printf("(index: %d) (ptr: %10p) (size: %ld)\n", i, curr, curr->memLen);
        curr = curr->next;
        i += 1;
    }
}


memNode *
memListAppend(memNode *header, memNode *node) {
    memNode *h;
    node->pre = NULL;
    if (header != NULL) {
        header->pre = node;
        node->next = header;
        h = node;
    } else {
        node->next = NULL;
        h = node;
    }
    return h;
}


memNode *
memListRemove(memNode *header, memNode *node) {
    memNode *h;
    memNode *curr;
    bool isInList;

    h = NULL;
    if (header == NULL) {
        printf("memListRemove header is NULL\n");
        return h;
    }

    // 查看 node 是不是在链表内, 若不在, 则非法修改内存地址
    isInList = false;
    curr = header;
    while (curr) {
        if (curr == node) {
            isInList = true;
            break;
        }
        curr = curr->next;
    }

    if (isInList == false) {
        printf("memListRemove remove node not in memList\n");
        return h;
    }

    // 移除对应的 node 供外界使用
    h = header;
    if (node->pre == NULL) {
        if (node->next) {
            h = node->next;
        } else {
            h = NULL;
        }
    } else {
        node->pre->next = node->next;
    }

    if (node->next) {
        node->next->pre = node->pre;
    }

    return h;
}


void
memListAppendTest() {
    int num = 10;
    printf("sbrk(0) %10p\n", sbrk(0));
    void *ptr = sbrk(num * sizeof(memNode));
    printf("sbrk(n) %10p\n", ptr);
    void *p = ptr;
    for (int i = 0; i < num; i++) {
        p = ptr_mem_head(p);
        printf("%10p  %ld\n", p, sizeof(memNode));
        memNodeList = memListAppend(memNodeList, (memNode *)p);
    }

    log_mem_list(memNodeList);
}


void
memListRemoveTest() {
    int num = 10;
    printf("sbrk(0) %10p\n", sbrk(0));
    void *ptr = sbrk(num * sizeof(memNode));
    printf("sbrk(n) %10p\n", ptr);
    void *p = ptr;
    for (int i = 0; i < num; i++) {
        p = ptr_mem_head(p);
        memNodeList = memListAppend(memNodeList, (memNode *)p);
    }

    // test
    log_mem_list(memNodeList);
    memNode *curr = memNodeList;
    while (curr && curr->next) {
        memNodeList = memListRemove(memNodeList, curr);
        printf("after remove\n");
        log_mem_list(memNodeList);
        // curr = curr->next;
        curr = curr->next->next;
    }
}


int
main(int argc, char *argv) {
    // memListAppendTest();
    memListRemoveTest();
}
