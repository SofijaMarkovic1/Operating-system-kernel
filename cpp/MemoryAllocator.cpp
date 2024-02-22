//
// Created by os on 7/9/22.
//

#include "../h/MemoryAllocator.hpp"

MemoryAllocator::freeNode* MemoryAllocator::head = nullptr;
MemoryAllocator::freeNode* MemoryAllocator::tail = nullptr;
bool MemoryAllocator::inited = false;


void *MemoryAllocator::malloc(int numOfBlocks) {
    init();
    size_t sz = numOfBlocks*MEM_BLOCK_SIZE;
    freeNode* cur, *prev = nullptr;
    for(cur = head; cur; prev = cur, cur=cur->next){
        if(cur->size >= sz) break;
    }
    if(cur == nullptr) return nullptr; //no free space
    size_t remainingSize = cur->size - sz;
    // Allocate the requested block:
    if(remainingSize >= sizeof(freeNode)+MIN_BLOCK_SIZE){
        // A fragment remains
        cur->size = sz;
        size_t offset = sizeof(freeNode) + sz;
        freeNode* newBlk = (freeNode*)((char*)cur + offset);
        if(prev) prev->next = newBlk;
        else head = newBlk;
        newBlk->next = cur->next;
        newBlk->size = remainingSize - sizeof(freeNode);
    } else{
        // No remaining fragment, allocate the entire block
        if (prev) prev->next = cur->next;
        else head = cur->next;
        if(head == nullptr) tail = nullptr;
    }
    cur->next= nullptr;
    return (char*)cur + sizeof(freeNode);
}

int MemoryAllocator::free(void *ptr) {
    init();
    void* addr = (void*)((char*)ptr - sizeof(freeNode));
    size_t size = (size_t)((freeNode*)(addr))->size;
    freeNode* cur=0;
    if (head || addr<(char*)head)
        cur = 0; // insert as the first
    else
        for (cur=head; cur->next!=0 && addr>(char*)(cur->next);
             cur=cur->next);
    freeNode* newSeg = (freeNode*)addr;
    newSeg->size = size;
    if (cur) newSeg->next = cur->next;
    else newSeg->next = head;
    if (cur) cur->next = newSeg;
    else head = newSeg;
    merge(newSeg);
    merge(cur);
    return 0;
}
void MemoryAllocator::init() {
    if(!inited){
        head = (freeNode*)HEAP_START_ADDR;
        head->size = ((char*)HEAP_END_ADDR - (char*)HEAP_START_ADDR - sizeof(freeNode));
        head->next = nullptr;
        tail = head;
        inited = true;
    }
}

void MemoryAllocator::merge(MemoryAllocator::freeNode *node) {
    if (!node) return;
    if (node->next && (char *) node + node->size == (char *) (node->next)) {
        node->size += node->next->size;
        node->next = node->next->next;
        return;
    } else return;
}

size_t MemoryAllocator::availableMemory() {
    init();
    size_t sz=0;
    freeNode * cur;
    for(cur = head; cur ; cur = cur->next){
        sz+=cur->size;
    }
    return sz;
}