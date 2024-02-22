//
// Created by os on 7/17/22.
//

#ifndef PROJECT_BASE_SCHEDULER_HPP
#define PROJECT_BASE_SCHEDULER_HPP

#include "MemoryAllocator.hpp"
class TCB;

class Scheduler {
private:
    static TCB* idle;

    static void initIdle();

    typedef struct Node{
        TCB* tcb;
        struct Node* next;
    }node;

    static node* head, *tail;

public:
    static TCB* get();

    static void put(TCB* tcb);

    static bool isEmpty() {
        return head == nullptr;
    }
};


#endif //PROJECT_BASE_SCHEDULER_HPP
