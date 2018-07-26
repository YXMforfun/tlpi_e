#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>


struct memNode {
    size_t memLen;
    struct memNode *pre;
    struct memNode *next;
};
typedef struct memNode memNodeHead;


static memNodeHead *memNodeList = NULL;


/*
    创建新的空闲内存链表表头, 在 free 的时候触发, 替换空闲内存链表表头
    以传入的指针为起始点, 往内存地址减小的方向移动(相当于回收堆里的内存)
*/
memNodeHead *
ptr_mem_head(void *ptr) {
    memNodeHead *p;
    p = (memNodeHead *)((unsigned long)ptr - sizeof(memNodeHead));
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


memNodeHead *
memListAppend(memNodeHead *header, memNode *node) {
    memNodeHead *h;
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


memNodeHead *
memListRemove(memNodeHead *header, memNode *node) {
    memNodeHead *h;
    memNode *curr;
    bool isInList;

    memNodeHead *h = NULL;
    if (header == NULL) {
        printf("memListRemove header is NULL\n");
        return h;
    }

    // 查看 node 是不是在链表内, 若不在, 则非法修改内存地址
    isInList = false;
    memNode *curr = header;
    while (curr) {
        if (curr == node) {
            isInList = true;
            break;
        }
    }

    if (isInList == false) {
        printf("memListRemove remove node not in memList\n");
        return h;
    }

    // 移除对应的 node 供外界使用




    if (node->pre != NULL) {
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
