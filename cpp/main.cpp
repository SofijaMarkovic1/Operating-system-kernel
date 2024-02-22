//
// Created by os on 7/9/22.
//
#include "../h/syscall_c.hpp"
#include "../test/printing.hpp"
#include "../h/TCB.hpp"
#include "../h/riscv.hpp"
#include "../h/console.hpp"
#include "../test/userMain.hpp"
//#include "../h/syscall_cpp.hpp"
extern "C" void supervisorTrap();
/*class Test : public PeriodicThread{
private:
    char c;
public:
    Test(char ch, time_t p) : PeriodicThread(p), c(ch){};
    void periodicActivation() override{
        putc(c);
        putc('\n');
    }
};*/
int main(){
    thread_t main;
    Riscv::w_stvec((uint64) &supervisorTrap);
    thread_create(&main, nullptr, nullptr);
    TCB::running = main;
    console::init();
    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);
    userMode();
    userMain();
    privilagedMode();
    console::waitTillTheEnd();
    return 0;
}
