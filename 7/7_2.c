#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


#ifndef ALLOC_SIZE
#define ALLOC_SIZE 3 * sysconf(_SC_PAGESIZE)
#endif

#ifndef DEALLOC_SIZE
#define DEALLOC_SIZE 1 * sysconf(_SC_PAGESIZE)
#endif

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


memNode *
memNodeSplit(memNode * node, size_t size) {
	void *ptr = ptr_mem_node(node);
	memNode *new_ptr = (memNode *)((unsigned long)ptr + size);
	new_ptr->memLen = node->memLen - (size + sizeof(memNode));
	node->memLen = size;
	return new_ptr;
}


void
memNodeMerge(memNode *header) {
    size_t node_size = sizeof(memNode);
    memNode *curr = header;
    unsigned long curr_v, next_v;
    unsigned long program_break = (unsigned long)sbrk(0);
    if (program_break == 0) {
        printf("memNodeMerge sbrk(0) failed\n");
        return;
    }

    while (curr && curr->next) {
        curr_v = (unsigned long)curr;
        next_v = (unsigned long)curr->next;
        if ((curr_v + curr->memLen + node_size) == next_v) {
            curr->memLen += curr->next->memLen + node_size;
            curr->next = curr->next->next;
            if (curr->next) {
                curr->next->pre = curr;
            } else {
                break;
            }
        }
        curr = curr->next;
    }

    curr_v = (unsigned long)curr;
    if ((curr_v + curr->memLen + node_size) == program_break && curr->memLen >= DEALLOC_SIZE) {
        memNodeList = memListRemove(memNodeList, curr);
        if (brk(curr) != 0) {
            printf("memNodeMerge brk(curr) failed\n");
        }
    }
}


void *
memNodeMalloc(size_t size) {
    void *ptr;
    memNode *curr, *new_node;
    size_t alloc_size;

    size_t node_size = sizeof(memNode);
    // 遍历空闲链表 看是否有合适大小的空闲内存块
    curr = memNodeList;
    while (curr) {
        if (curr->memLen >= size + node_size) {
            ptr = ptr_mem_node(curr);
            memNodeList = memListRemove(memNodeList, curr);
            // 如果刚好大小合适, 就直接返回内存地址
            if (curr->memLen == size) {
                return ptr;
            }
            // 不然就根据大小切割, 多余部分放回 空闲内存链表
            new_node = memNodeSplit(curr, size);
            memNodeList = memListAppend(memNodeList, new_node);
            return ptr;
        } else {
            curr = curr->next;
        }
    }

    // 没有合适的空闲内存块, 通过 sbrk() 开辟新的内存块(堆)
    if (size >= ALLOC_SIZE) {
        alloc_size = size + node_size;
    } else {
        alloc_size = ALLOC_SIZE;
    }

    ptr = sbrk(alloc_size);
    if (ptr == NULL) {
        printf("memNodeMalloc sbrk failed\n");
        return NULL;
    }
    curr = (memNode *)ptr;
    curr->next = NULL;
    curr->pre = NULL;
    curr->memLen = alloc_size - node_size;
    if (alloc_size > size + node_size) {
        new_node = memNodeSplit(curr, size);
        memNodeList = memListAppend(memNodeList, new_node);
    }
    return ptr_mem_node(ptr);
}


void
memNodeFree(void *ptr) {
    ptr = ptr_mem_head(ptr);
    memNodeList = memListAppend(memNodeList, (memNode *)ptr);
    if (memNodeList == NULL) {
        printf("memNodeFree failed\n");
        return;
    }
    memNodeMerge(memNodeList);
    return;
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


void
mallocFreeTest() {
    char *str;
    printf("sbrk(0) %10p\n", sbrk(0));
    str = (char *)memNodeMalloc(10);
    printf("sbrk(0) %10p\n", sbrk(0));
    printf("str %10p\n", str);
    log_mem_list(memNodeList);
    strcpy(str, "abcdefg");
    printf("str %s, address %10p\n", str, str);
    log_mem_list(memNodeList);
    memNodeFree(str);
    log_mem_list(memNodeList);
    printf("sbrk(0) %10p\n", sbrk(0));
}


int
main(int argc, char *argv) {
    // memListAppendTest();
    // memListRemoveTest();
    mallocFreeTest();
}
