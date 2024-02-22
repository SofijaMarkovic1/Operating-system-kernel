//
// Created by os on 7/17/22.
//

#include "../h/TCB.hpp"
#include "../h/riscv.hpp"
TCB *TCB::running = nullptr;
uint64 TCB::timeSliceCounter = 0;
TCB::sleepNode* TCB::sleepHead= nullptr;
TCB::sleepNode* TCB::sleepTail= nullptr;

void TCB::yield()
{
    __asm__ volatile("li a0, 0x13");
    __asm__ volatile("ecall");
}

void TCB::dispatch()
{
    TCB *old = running;
    if (!old->isFinished() && !old->sleeping) { Scheduler::put(old); }
    running = Scheduler::get();

    TCB::contextSwitch(&old->context, &running->context);
}

int TCB::createThread(TCB **handle, TCB::Body start_routine, void* arg, uint64 *stack) {
    TCB* newTCB = (TCB*)MemoryAllocator::malloc(MemoryAllocator::bytesToBlocks(sizeof(TCB)));
    if(newTCB == nullptr) {
        if(stack) MemoryAllocator::free(stack);
        return -1;
    }
    if(start_routine == nullptr){
        newTCB->semErr = false;
        newTCB->cppThread = false;
        newTCB->started = true;
        newTCB->sleeping= false;
        newTCB->context.sp = 0;
        newTCB->context.ra = 0;
        newTCB->body = nullptr;
        newTCB->finished = false;
        newTCB->stack = nullptr;
        newTCB->timeSlice = DEFAULT_TIME_SLICE;
        newTCB->arg = nullptr;
        *handle = newTCB;
        return 0;
    } else{
        newTCB->semErr = false;
        newTCB->cppThread = false;
        newTCB->started = true;
        newTCB->sleeping= false;
        newTCB->stack = stack;
        newTCB->finished = false;
        newTCB->body = start_routine;
        newTCB->context.sp = (uint64) &stack[DEFAULT_STACK_SIZE];
        newTCB->context.ra = (uint64) thread_wrapper;
        newTCB->arg = arg;
        newTCB->timeSlice = DEFAULT_TIME_SLICE;
        Scheduler::put(newTCB);
        *handle = newTCB;
        return 0;
    }
}

void TCB::thread_wrapper(void *arg) {
    Riscv::popSppSpie();
    running->body(running->arg);
    running->setFinished(true);
    yield();
}

void TCB::kill() {
    running->setFinished(true);
    yield();
}


int TCB::sleep(time_t t) {
    if(TCB::running == nullptr) return -1;
    if(t == 0) return 0;
    sleepNode* sn = (sleepNode*)MemoryAllocator::malloc(MemoryAllocator::bytesToBlocks(sizeof(sleepNode)));
    sn->thread = TCB::running;
    if(sleepHead == nullptr){
        sn->timeLeft = t;
        sn->next = nullptr;
        sleepHead = sn;
        sleepTail = sn;
    }
    else{
        int relT = t;
        sleepNode* cur=sleepHead, *prev= nullptr;
        while(cur){
            if((int)relT - (int)cur->timeLeft < 0) break;
            relT-=(int)cur->timeLeft;
            prev = cur;
            cur = cur->next;
        }
        if(cur == nullptr){ //add last
            sleepTail->next=sn;
            sleepTail = sleepTail->next;
            sn->timeLeft = relT;
            sn->next = nullptr;
        }
        else if(cur == sleepHead){ //add first
            sn->timeLeft=relT;
            cur->timeLeft-=relT;
            sn->next = sleepHead;
            sleepHead = sn;
        }
        else{ //add in the middle
            /*if(relT-cur->timeLeft == 0){ //after cur
                sn->next = cur->next;
                cur->next=sn;
                sn->timeLeft=0;*/
            //}else { //before cur
                prev->next = sn;
                sn->next = cur;
                sn->timeLeft = relT;
                cur->timeLeft -= relT;
            //}
        }
    }
    running->sleeping = true;
    dispatch();
    return 0;
}

int TCB::wakeUp() {
    if(sleepHead == nullptr) return -1;
    sleepHead->timeLeft--;
    if(sleepHead->timeLeft==0){
        sleepNode* prev = nullptr;
        while(sleepHead && sleepHead->timeLeft==0){
            prev = sleepHead;
            sleepHead = sleepHead->next;
            if(sleepHead == nullptr) sleepTail = nullptr;
            prev->next = nullptr;
            prev->thread->sleeping = false;
            Scheduler::put(prev->thread);
            MemoryAllocator::free(prev);
        }
    }
    return 0;
}

void TCB::idleBody(void *arg) {
    privilagedMode();
    Riscv::popSppSpie();
    while (true){
        if(!Scheduler::isEmpty()) yield();
    }
}

int TCB::createCppThread(TCB** handle, Body start_routine, void* arg, uint64* stack) {
    TCB* t = (TCB*)MemoryAllocator::malloc(MemoryAllocator::bytesToBlocks(sizeof(TCB)));
    if(t == nullptr) return -1;
    t->timeSlice = DEFAULT_TIME_SLICE;
    t->body = start_routine;
    t->stack = stack;
    t->context.sp = (uint64)&stack[DEFAULT_STACK_SIZE];
    t->context.ra = (uint64)&thread_wrapper;
    t->finished = false;
    t->arg = arg;
    t->cppThread=true;
    t->started=false;
    t->semErr = false;
    t->sleeping = false;
    *handle=t;
    return 0;
}

int TCB::startCppThread(TCB *t) {
    if(t == nullptr) return -2;
    if(!t->cppThread || t->started) return -1;
    else{
        t->started = true;
        Scheduler::put(t);
        return 0;
    }
}
