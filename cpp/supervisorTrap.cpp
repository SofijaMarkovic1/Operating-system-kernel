//
// Created by os on 7/9/22.
//
#include "../lib/console.h"
#include "../lib/hw.h"
#include "../h/MemoryAllocator.hpp"
#include "../h/riscv.hpp"
#include "../h/TCB.hpp"
#include "../h/Sem.hpp"
#include "../h/console.hpp"
#include "../test/printing.hpp"
extern "C" void handleSupervisorTrap() {
    uint64 a0, a1, a2, a3, a4, a5, a6, a7;

    //reading arguments
    __asm__ volatile("sd a7, %0":"=m"(a7));
    __asm__ volatile("sd a6, %0":"=m"(a6));
    __asm__ volatile("sd a5, %0":"=m"(a5));
    __asm__ volatile("sd a4, %0":"=m"(a4));
    __asm__ volatile("sd a3, %0":"=m"(a3));
    __asm__ volatile("sd a2, %0":"=m"(a2));
    __asm__ volatile("sd a1, %0":"=m"(a1));
    __asm__ volatile("sd a0, %0":"=m"(a0));

    //reading scause, sscratch, sepc
    uint64 volatile scauseVar = Riscv::r_scause();
    uint64 volatile sscratchVar;
    __asm__ volatile("csrr %0, sscratch" : "=r" (sscratchVar));
    uint64 volatile sepcVar = Riscv::r_sepc();
    uint64 volatile sstatus = Riscv::r_sstatus();
    bool flag = false;
    if(scauseVar == 0x09 || scauseVar == 0x08){
        switch (a0) {
            case 0x01:{ // void* mem_alloc (size_t size);
                void* ret = MemoryAllocator::malloc(MemoryAllocator::bytesToBlocks((size_t)a1));
                __asm__ volatile("mv a0, %0" : : "r" (ret));
                break;
            }
            case 0x02:{ //int mem_free (void*);
                int ret = MemoryAllocator::free((void*)a1);
                __asm__ volatile("mv a0, %0" : : "r" (ret));
                break;
            }
            case 0x11:{ //int thread_create (thread_t* handle, void(*start_routine)(void*), void* arg);
                int ret = TCB::createThread((TCB**)a1, (TCB::Body)a2, (void*)a3, (uint64*)a4);
                __asm__ volatile("mv a0, %0" : : "r" (ret));
                break;
            }
            case 0x12:{ //int thread_exit ();
                int ret = 0;
                if(TCB::running== nullptr) ret = -1;
                TCB::kill();
                __asm__ volatile("mv a0, %0" : : "r" (ret));
                break;
            }
            case 0x13: { //void thread_dispatch ();
                //uint64 sepcVar1 = Riscv::r_sepc();
                //sepcVar1+=4;
                TCB::timeSliceCounter = 0;
                TCB::dispatch();
                break;
            }
            case 0x14:{ //int createCPPthread(thread_t* handle, void(*start_routine)(void*), void* arg);
                int ret = TCB::createCppThread((TCB**)a1, (TCB::Body)a2, (void*)a3, (uint64*)a4);
                __asm__ volatile("mv a0, %0" : : "r" (ret));
                break;
            }
            case 0x15:{ //int startCPPthread(thread_t handle);
                int ret = TCB::startCppThread((TCB*)a1);
                __asm__ volatile("mv a0, %0" : : "r" (ret));
                break;
            }
            case 0x21:{ //int sem_open (sem_t* handle, unsigned init)
                int ret = Sem::open((Sem**)a1, (unsigned) a2);
                __asm__ volatile("mv a0, %0" : : "r" (ret));
                break;
            }
            case 0x22:{ //int sem_close (sem_t handle)
                int ret = Sem::close((Sem*)a1);
                __asm__ volatile("mv a0, %0" : : "r" (ret));
                break;
            }
            case 0x23:{ //int sem_wait (sem_t id)
                int ret = Sem::wait((Sem*)a1);
                __asm__ volatile("mv a0, %0" : : "r" (ret));
                break;
            }
            case 0x24:{ //int sem_signal (sem_t id)
                int ret = Sem::signal((Sem*)a1);
                __asm__ volatile("mv a0, %0" : : "r" (ret));
                break;
            }
            case 0x31:{ //int time_sleep (time_t);
                int ret = TCB::sleep((time_t)a1);
                __asm__ volatile("mv a0, %0" : : "r" (ret));
                break;
            }
            case 0x41:{ //char getc ();
                char ret = console::inputBuffer->get();
                __asm__ volatile("mv a0, %0" : : "r" (ret));
                break;
            }
            case 0x42:{ //void putc (char);
                console::outputBuffer->put((char)a1);
                break;
            }
            case 0x71:{ //void privilagedMode();
                uint64 temp;
                asm volatile("csrr %0, sstatus" : "=r" (temp));
                temp |= 0x100;
                asm volatile("csrw sstatus, %0" : : "r" (temp));
                flag = true;
                break;
            }
            case 0x72:{ //void userMode();
                uint64 temp;
                asm volatile("csrr %0, sstatus" : "=r" (temp));
                temp &= ~0x100;
                asm volatile("csrw sstatus, %0" : : "r" (temp));
                flag = true;
                break;
            }
        }
        __asm__ volatile("sd a0, 0x50(%0)" : : "r" (sscratchVar)); //writing over a0 on stack
        if(!flag) Riscv::w_sstatus(sstatus);
        //sepcVar = sepcVar + 4; //ecall doesn't increment pc
        Riscv::w_sepc(sepcVar+4);
    }
    else if(scauseVar == 0x8000000000000001UL){
        TCB::timeSliceCounter++;
        TCB::wakeUp();
        if(TCB::timeSliceCounter == TCB::running->getTimeSlice()){
            uint64 sepc = Riscv::r_sepc();
            uint64 sstatus = Riscv::r_sstatus();
            TCB::timeSliceCounter = 0;
            TCB::dispatch();
            Riscv::w_sstatus(sstatus);
            Riscv::w_sepc(sepc);
        }
        Riscv::mc_sip(Riscv::SIP_SSIE);
    }
    else if(scauseVar == 0x8000000000000009UL){
        if(plic_claim() == CONSOLE_IRQ){
            plic_complete(CONSOLE_IRQ);
        }
    }
    else {
        //print scause
        //print sepc
        //print stval
    }
    //console_handler();
}