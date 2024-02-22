//
// Created by os on 7/17/22.
//

#ifndef PROJECT_BASE_TCB_HPP
#define PROJECT_BASE_TCB_HPP
#include "../lib/hw.h"
#include "MemoryAllocator.hpp"
#include "Scheduler.hpp"
#include "syscall_c.hpp"


class TCB {
    friend class Sem;
public:

    bool isFinished() const { return finished; }

    void setFinished(bool value) { finished = value; }

    using Body = void (*)(void*);

    static int createThread(TCB** handle, Body start_routine, void* arg, uint64* stack);

    static void yield();

    static TCB *running;

    static void kill();

    uint64 getTimeSlice() const {
        return timeSlice;
    }
    void setTimeSlice(uint64 timeSlice) {
        TCB::timeSlice = timeSlice;
    }

    static void dispatch();

    static uint64 timeSliceCounter;

    static int sleep(time_t t);

    static int wakeUp();

    static void idleBody(void * arg);

    static int createCppThread(TCB** handle, Body start_routine, void* arg, uint64* stack);

    static int startCppThread(TCB *t);

private:
    struct Context
    {
        uint64 ra;
        uint64 sp;
    };

    Body body;
    uint64 *stack;
    Context context;
    bool finished;
    void* arg;
    uint64 timeSlice;

    typedef struct sleepNode{
        time_t timeLeft;
        //long timeLeft;
        TCB* thread;
        struct sleepNode* next;
    }sleepNode;
    static sleepNode* sleepHead, *sleepTail;

    bool cppThread;
    bool started;
    bool semErr;
    static void contextSwitch(Context *oldContext, Context *runningContext);


    static void thread_wrapper(void* arg);
    static void checkAll();
public:
    bool sleeping;
};


#endif //PROJECT_BASE_TCB_HPP
