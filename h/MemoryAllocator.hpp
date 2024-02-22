//
// Created by os on 7/9/22.
//

#ifndef PROJECT_BASE_MEMORYALLOCATOR_HPP
#define PROJECT_BASE_MEMORYALLOCATOR_HPP
#include "../lib/hw.h"
#include "../lib/console.h"

class MemoryAllocator {
private:
    typedef struct freeNode{
        size_t size;
        struct freeNode* next;
    }freeNode;
    static constexpr size_t MIN_BLOCK_SIZE=64;
    static freeNode* head, *tail;
    static void init();
    static bool inited;
    static void merge(freeNode* node);
public:
    static int bytesToBlocks(size_t bytes){
        int num = bytes/MEM_BLOCK_SIZE;
        if(bytes%MEM_BLOCK_SIZE!=0) num++;
        return num;
    }
    static void* malloc(int numOfBlocks);
    static int free(void* ptr);
    static size_t availableMemory();
};


#endif //PROJECT_BASE_MEMORYALLOCATOR_HPP
