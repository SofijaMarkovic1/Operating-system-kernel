//
// Created by os on 7/24/22.
//

#ifndef PROJECT_BASE_SYSCALL_CPP_HPP
#define PROJECT_BASE_SYSCALL_CPP_HPP

#endif //PROJECT_BASE_SYSCALL_CPP_HPP
#include "../h/syscall_c.hpp"
void *operator new(size_t n);

void *operator new[](size_t n);

void operator delete(void *p) noexcept;

void operator delete[](void *p) noexcept;


class Thread {
public:
    Thread (void (*body)(void*), void* arg);
    virtual ~Thread ();
    int start ();
    static void dispatch ();
    static int sleep (time_t t);
protected:
    Thread ();
    virtual void run () {}
private:
    static void thread_wrapper(void* arg);
    thread_t myHandle;
};


class Semaphore {
public:
    Semaphore (unsigned init = 1);
    virtual ~Semaphore ();
    int wait ();
    int signal ();
private:
    sem_t myHandle;
};


class PeriodicThread : public Thread {
protected:
    PeriodicThread (time_t period);
    virtual void periodicActivation () {}

private:
    typedef struct Arguments{
        PeriodicThread* thread;
        time_t period;
        Arguments(PeriodicThread* t, time_t p): thread(t), period(p){}
    }Args;

    static void thread_wrapperP(void* arg);
};
class Console {
public:
    static char getc ();
    static void putc (char c);
};