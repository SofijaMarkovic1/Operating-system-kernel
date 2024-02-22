//
// Created by os on 7/10/22.
//

#include "../h/syscall_c.hpp"
#include "../h/MemoryAllocator.hpp"

void *mem_alloc(size_t size) {
    __asm__ volatile("mv a1, %0" : : "r" (size));
    __asm__ volatile("li a0, 0x01");

    __asm__ volatile("ecall");

    void* ret;
    __asm__ volatile("mv %0, a0" : "=r" (ret));
    return ret;
}

int mem_free(void *ptr) {
    __asm__ volatile("mv a1, %0" : : "r" (ptr));
    __asm__ volatile("li a0, 0x02");

    __asm__ volatile("ecall");

    int ret;
    __asm__ volatile("mv %0, a0" : "=r" (ret));
    return ret;
}

int thread_create (thread_t* handle, void(*start_routine)(void*), void* arg){
    uint64* stack = nullptr;
    if(start_routine != nullptr){
        stack = (uint64*) MemoryAllocator::malloc(MemoryAllocator::bytesToBlocks(DEFAULT_STACK_SIZE));
    }
    __asm__ volatile("mv a4, %0" : : "r" (stack));
    __asm__ volatile("mv a3, %0" : : "r" (arg));
    __asm__ volatile("mv a2, %0" : : "r" (start_routine));
    __asm__ volatile("mv a1, %0" : : "r" (handle));
    __asm__ volatile("li a0, 0x11");

    __asm__ volatile("ecall");

    int ret;
    __asm__ volatile("mv %0, a0" : "=r" (ret));
    return ret;
}

int thread_exit (){
    __asm__ volatile("li a0, 0x12");

    __asm__ volatile("ecall");

    int ret;
    __asm__ volatile("mv %0, a0" : "=r" (ret));
    return ret;
}

void thread_dispatch (){
    __asm__ volatile("li a0, 0x13");

    __asm__ volatile("ecall");
}

int sem_open (sem_t* handle, unsigned init){
    __asm__ volatile("mv a2, %0" : : "r" (init));
    __asm__ volatile("mv a1, %0" : : "r" (handle));
    __asm__ volatile("li a0, 0x21");

    __asm__ volatile("ecall");

    int ret;
    __asm__ volatile("mv %0, a0" : "=r" (ret));
    return ret;
}

int sem_close (sem_t handle){
    __asm__ volatile("mv a1, %0" : : "r" (handle));
    __asm__ volatile("li a0, 0x22");

    __asm__ volatile("ecall");

    int ret;
    __asm__ volatile("mv %0, a0" : "=r" (ret));
    return ret;
}

int sem_wait (sem_t id){
    __asm__ volatile("mv a1, %0" : : "r" (id));
    __asm__ volatile("li a0, 0x23");

    __asm__ volatile("ecall");

    int ret;
    __asm__ volatile("mv %0, a0" : "=r" (ret));
    return ret;
}

int sem_signal (sem_t id){
    __asm__ volatile("mv a1, %0" : : "r" (id));
    __asm__ volatile("li a0, 0x24");

    __asm__ volatile("ecall");

    int ret;
    __asm__ volatile("mv %0, a0" : "=r" (ret));
    return ret;
}

int time_sleep (time_t t){
    __asm__ volatile("mv a1, %0" : : "r" (t));
    __asm__ volatile("li a0, 0x31");

    __asm__ volatile("ecall");

    int ret;
    __asm__ volatile("mv %0, a0" : "=r" (ret));
    return ret;
}

char getc (){
    __asm__ volatile("li a0, 0x41");

    __asm__ volatile("ecall");

    char ret;
    __asm__ volatile("mv %0, a0" : "=r" (ret));
    return ret;
}

void putc (char c){
    __asm__ volatile("mv a1, %0" : : "r" (c));
    __asm__ volatile("li a0, 0x42");

    __asm__ volatile("ecall");
}

int createCPPthread(thread_t* handle, void(*start_routine)(void*), void* arg) {
    uint64* stack = nullptr;
    if(start_routine != nullptr){
        stack = (uint64*) MemoryAllocator::malloc(MemoryAllocator::bytesToBlocks(DEFAULT_STACK_SIZE));
    }
    __asm__ volatile("mv a4, %0" : : "r" (stack));
    __asm__ volatile("mv a3, %0" : : "r" (arg));
    __asm__ volatile("mv a2, %0" : : "r" (start_routine));
    __asm__ volatile("mv a1, %0" : : "r" (handle));
    __asm__ volatile("li a0, 0x14");

    __asm__ volatile("ecall");

    int ret;
    __asm__ volatile("mv %0, a0" : "=r" (ret));
    return ret;

}

int startCPPthread(thread_t handle) {
    __asm__ volatile("mv a1, %0" : : "r" (handle));
    __asm__ volatile("li a0, 0x15");

    __asm__ volatile("ecall");

    int ret;
    __asm__ volatile("mv %0, a0" : "=r" (ret));
    return ret;
}

void privilagedMode() {
    __asm__ volatile("li a0, 0x71");

    __asm__ volatile("ecall");
}

void userMode() {
    __asm__ volatile("li a0, 0x72");

    __asm__ volatile("ecall");
}
