//
// Created by os on 7/17/22.
//

#include "../h/Scheduler.hpp"
#include "../h/TCB.hpp"
Scheduler::node * Scheduler::head = nullptr;
Scheduler::node * Scheduler::tail = nullptr;
TCB* Scheduler::idle = nullptr;

TCB *Scheduler::get() {
    if(head == nullptr) {
        if(idle == nullptr) initIdle();
        return idle;
    }
    node* ret = head;
    head = head->next;
    if(head == nullptr) tail = nullptr;
    ret->next = nullptr;
    TCB* tcb = ret->tcb;
    MemoryAllocator::free(ret);
    return tcb;
}

void Scheduler::put(TCB *tcb) {
    if(tcb == idle) return;
    node * newNode = (Node*)MemoryAllocator::malloc(MemoryAllocator::bytesToBlocks(sizeof(node)));
    newNode->next = nullptr;
    newNode->tcb = tcb;
    if(head == nullptr){
        head = newNode;
        tail = newNode;
    }
    else{
        tail->next = newNode;
        tail = newNode;
    }
    return;
}

void Scheduler::initIdle() {
    uint64 * stack = (uint64*) MemoryAllocator::malloc(MemoryAllocator::bytesToBlocks(DEFAULT_STACK_SIZE));
    TCB::createThread(&idle, TCB::idleBody, nullptr, stack);
}
