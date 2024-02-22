//
// Created by os on 7/22/22.
//

#ifndef PROJECT_BASE_SEM_HPP
#define PROJECT_BASE_SEM_HPP
#include "TCB.hpp"
#include "MemoryAllocator.hpp"
#include "riscv.hpp"
#include "../lib/console.h"
class Sem {
public:

    static int open(Sem** handle, unsigned init);
    static int close(Sem* handle);
    static int  wait (Sem* handle);
    static int signal (Sem* handle);
    int value () const { return val; }
protected:
    static void block (Sem* handle);
    static void unblock (Sem* handle, int flag=0);
private:
    int val;
    typedef struct Node{
        struct Node* next;
        TCB* thread;
    }node;
    node* head, *tail; //blocked list
};


#endif //PROJECT_BASE_SEM_HPP