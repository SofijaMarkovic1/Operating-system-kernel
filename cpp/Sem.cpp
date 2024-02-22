//
// Created by os on 7/22/22.
//

#include "../h/Sem.hpp"

int Sem::open(Sem **handle, unsigned init) {
    Sem* sem = (Sem*) MemoryAllocator::malloc(MemoryAllocator::bytesToBlocks(sizeof(Sem)));
    if(sem == nullptr) return -1;
    sem->val = init;
    sem->head = nullptr;
    sem->tail = nullptr;
    *handle = sem;
    return 0;
}



int Sem::close(Sem *handle) {
    if(handle == nullptr) return -1;
    if(handle->head) {
        while (handle->head) {
            unblock(handle, -1);
        }
    }
    MemoryAllocator::free(handle);
    return 0;
}

int Sem::wait(Sem* handle) {
    if(handle == nullptr) return -1;
    if (--handle->val < 0) block(handle);
    if(TCB::running->semErr){
        TCB::running->semErr = false;
        return -1;
    }
    else return 0;
}

int Sem::signal(Sem* handle) {
    if(handle == nullptr) return -1;
    if (++handle->val <= 0) unblock(handle);
    return 0;
}

void Sem::block(Sem* handle) {
    TCB* old = TCB::running;
    TCB::running = Scheduler::get();
    node* n = (node*)MemoryAllocator::malloc(MemoryAllocator::bytesToBlocks(sizeof(node)));
    n->thread = old;
    n->next = nullptr;
    if(handle->tail) {
        handle->tail->next = n;
        handle->tail = n;
    }
    else{
        handle->head = handle->tail = n;
    }
    //old->blocked = true;
    //TCB::dispatch();
    Riscv::pushRegisters();
    TCB::contextSwitch(&old->context, &TCB::running->context);
    Riscv::popRegisters();
}

void Sem::unblock(Sem* handle, int flag) {
    if(handle == nullptr) return;
    if(handle->head == nullptr) return;
    TCB* t = handle->head->thread;
    if(t && flag == -1) t->semErr = true;
    if(t) {
        Scheduler::put(t);
    }
    node* old = handle->head;
    handle->head = handle->head->next;
    if(handle->head == nullptr) handle->tail = nullptr;
    old->next = nullptr;
    MemoryAllocator::free(old);
}
